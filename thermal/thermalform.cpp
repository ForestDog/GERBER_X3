// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*******************************************************************************
*                                                                              *
* Author    :  Damir Bakiev                                                    *
* Version   :  na                                                              *
* Date      :  01 February 2020                                                *
* Website   :  na                                                              *
* Copyright :  Damir Bakiev 2016-2020                                          *
*                                                                              *
* License:                                                                     *
* Use, modification & distribution is subject to Boost Software License Ver 1. *
* http://www.boost.org/LICENSE_1_0.txt                                         *
*                                                                              *
*******************************************************************************/
#include "thermalform.h"
#include "../gcodepropertiesform.h"
#include "gbrfile.h"
#include "gcode.h"
#include "gi/bridgeitem.h"
#include "graphicsview.h"
#include "myclipper.h"
#include "project.h"
#include "scene.h"
#include "settings.h"
#include "thermaldelegate.h"
#include "thermalmodel.h"
#include "thermalnode.h"
#include "thermalpreviewitem.h"
#include "tooldatabase/tooldatabase.h"
#include "tooldatabase/tooleditdialog.h"
#include "ui_thermalform.h"
#include <QCheckBox>
#include <QDockWidget>
#include <QFuture>
#include <QItemSelection>
#include <QMessageBox>
#include <QPicture>
#include <QTimer>
#include <QtConcurrent>

#include "leakdetector.h"

enum { Size = 24 };

extern QIcon drawApertureIcon(Gerber::AbstractAperture* aperture);

QIcon drawRegionIcon(const Gerber::GraphicObject& go)
{
    static QMutex m;
    QMutexLocker l(&m);

    QPainterPath painterPath;

    for (QPolygonF polygon : go.paths())
        painterPath.addPolygon(polygon);

    const QRectF rect = painterPath.boundingRect();

    qreal scale = static_cast<double>(Size) / qMax(rect.width(), rect.height());

    double ky = rect.bottom() * scale;
    double kx = rect.left() * scale;
    if (rect.width() > rect.height())
        ky += (static_cast<double>(Size) - rect.height() * scale) / 2;
    else
        kx -= (static_cast<double>(Size) - rect.width() * scale) / 2;

    QPixmap pixmap(Size, Size);
    pixmap.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    //    painter.translate(tr);
    painter.translate(-kx, ky);
    painter.scale(scale, -scale);
    painter.drawPath(painterPath);
    return QIcon(pixmap);
}

ThermalForm::ThermalForm(QWidget* parent)
    : FormsUtil(new GCode::ThermalCreator, parent)
    , ui(new Ui::ThermalForm)
{
    ui->setupUi(this);

    MySettings settings;
    settings.beginGroup("ThermalForm");
    settings.getValue(varName(par.angle), 0.0);
    settings.getValue(varName(par.count), 4);
    settings.getValue(varName(par.tickness), 0.5);
    lastMax = settings.getValue(ui->dsbxAreaMax, 10.0);
    lastMin = settings.getValue(ui->dsbxAreaMin);
    settings.getValue(ui->chbxIgnoreCopper);
    settings.getValue(ui->chbxAperture, true);
    settings.getValue(ui->chbxPath);
    settings.getValue(ui->chbxPour);
    settings.endGroup();

    parent->setWindowTitle(ui->label->text());

    ui->pbClose->setIcon(QIcon::fromTheme("window-close"));
    ui->pbCreate->setIcon(QIcon::fromTheme("document-export"));

    for (QPushButton* button : findChildren<QPushButton*>())
        button->setIconSize({ 16, 16 });

    connect(ui->pbClose, &QPushButton::clicked, dynamic_cast<QWidget*>(parent), &QWidget::close);
    connect(ui->pbCreate, &QPushButton::clicked, this, &ThermalForm::createFile);
    connect(ui->toolHolder, &ToolSelectorForm::updateName, this, &ThermalForm::updateName);

    ui->treeView->setIconSize(QSize(Size, Size));
    connect(ui->treeView, &QTreeView::clicked, ui->treeView, qOverload<const QModelIndex&>(&QTreeView::edit));

    connect(ui->chbxAperture, &QCheckBox::toggled, [this] { createTPI(nullptr); });
    connect(ui->chbxPath, &QCheckBox::toggled, [this] { createTPI(nullptr); });
    connect(ui->chbxPour, &QCheckBox::toggled, [this] { createTPI(nullptr); });

    updateName();

    if (0) {
        chbx = new QCheckBox("", ui->treeView);
        chbx->setMinimumHeight(ui->treeView->header()->height() - 4);
        chbx->setEnabled(false);
        auto lay = new QGridLayout(ui->treeView->header());
        lay->addWidget(chbx, 0, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);
        lay->setContentsMargins(3, 0, 0, 0);
    }

    updateFiles();

    if (ui->cbxFile->count() < 1)
        return;

    {
        int w = ui->treeView->indentation();
        int h = Size; //QFontMetrics(font()).height(); // ui->treeView->rowHeight(m_model->index(0, 0, QModelIndex()));
        QImage i(w, h, QImage::Format_ARGB32);
        i.fill(Qt::transparent);
        for (int y = 0; y < h; ++y)
            i.setPixelColor(w / 2, y, QColor(128, 128, 128));
        i.save("vline.png", "PNG");

        for (int x = w / 2; x < w; ++x)
            i.setPixelColor(x, h / 2, QColor(128, 128, 128));
        i.save("branch-more.png", "PNG");

        i.fill(Qt::transparent);
        for (int y = 0; y < h / 2; ++y)
            i.setPixelColor(w / 2, y, QColor(128, 128, 128));
        for (int x = w / 2; x < w; ++x)
            i.setPixelColor(x, h / 2, QColor(128, 128, 128));
        i.save("branch-end.png", "PNG");

        QFile file(":/qtreeviewstylesheet/QTreeView.qss");
        file.open(QFile::ReadOnly);
        ui->treeView->setUniformRowHeights(true);
        ui->treeView->setStyleSheet(file.readAll());
        ui->treeView->header()->setMinimumHeight(h);
        ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->treeView->header()->setStretchLastSection(false);
        ui->treeView->hideColumn(1);
        ui->treeView->setItemDelegate(new ThermalDelegate(this));
    }
}

ThermalForm::~ThermalForm()
{
    qDebug(Q_FUNC_INFO);
    par = model->rootItem->child(0)->getParam();
    MySettings settings;
    settings.beginGroup("ThermalForm");
    settings.setValue(varName(par.angle));
    settings.setValue(varName(par.count));
    settings.setValue(varName(par.tickness));
    settings.setValue(ui->dsbxAreaMax);
    settings.setValue(ui->dsbxAreaMin);
    settings.setValue(ui->chbxIgnoreCopper);
    settings.setValue(ui->chbxAperture);
    settings.setValue(ui->chbxPath);
    settings.setValue(ui->chbxPour);
    settings.endGroup();
    delete ui;
}

void ThermalForm::updateFiles()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    disconnect(ui->cbxFile, qOverload<int>(&QComboBox::currentIndexChanged), this, &ThermalForm::on_cbxFileCurrentIndexChanged);
#else
    disconnect(ui->cbxFile, qOverload<int /*, const QString&*/>(&QComboBox::currentIndexChanged), this, &ThermalForm::on_cbxFileCurrentIndexChanged);
#endif
    ui->cbxFile->clear();

    for (Gerber::File* file : App::project()->files<Gerber::File>()) {
        if (file->flashedApertures()) {
            ui->cbxFile->addItem(file->shortName(), file->id());
            QPixmap pixmap(Size, Size);
            QColor color(file->color());
            color.setAlpha(255);
            pixmap.fill(color);
            ui->cbxFile->setItemData(ui->cbxFile->count() - 1, QIcon(pixmap), Qt::DecorationRole);
            ui->cbxFile->setItemData(ui->cbxFile->count() - 1, QSize(0, Size), Qt::SizeHintRole);
        }
    }

    on_cbxFileCurrentIndexChanged(0);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(ui->cbxFile, qOverload<int>(&QComboBox::currentIndexChanged), this, &ThermalForm::on_cbxFileCurrentIndexChanged);
#else
    connect(ui->cbxFile, qOverload<int /*, const QString&*/>(&QComboBox::currentIndexChanged), this, &ThermalForm::on_cbxFileCurrentIndexChanged);
#endif
}

bool ThermalForm::canToShow()
{
    for (Gerber::File* file : App::project()->files<Gerber::File>())
        if (file->flashedApertures())
            return true;

    QMessageBox::information(nullptr, "", tr("No data to process."));
    return false;
}

void ThermalForm::on_leName_textChanged(const QString& arg1) { m_fileName = arg1; }

void ThermalForm::createFile()
{
    if (!tool.isValid()) {
        tool.errorMessageBox(this);
        return;
    }

    Paths wPaths;
    Pathss wBridgePaths;

    for (QSharedPointer<ThermalPreviewItem> item : m_sourcePreview) {
        if (item->isValid()) {
            wPaths.append(item->paths());
            wBridgePaths.append(item->bridge());
        }
    }

    GCode::GCodeParams gpc;
    gpc.setConvent(true);
    gpc.setSide(GCode::Outer);
    gpc.tools.append(tool);
    gpc.params[GCode::GCodeParams::Depth] = ui->dsbxDepth->value();
    gpc.params[GCode::GCodeParams::FileId] = ui->cbxFile->currentData().toInt();
    gpc.params[GCode::GCodeParams::IgnoreCopper] = ui->chbxIgnoreCopper->isChecked();
    m_tpc->setGcp(gpc);
    m_tpc->addPaths(wPaths);
    m_tpc->addSupportPaths(wBridgePaths);
    createToolpath();
}

void ThermalForm::updateName()
{
    tool = ui->toolHolder->tool();
    ui->leName->setText(tr("Thermal"));
    redraw();
}

void ThermalForm::on_cbxFileCurrentIndexChanged(int /*index*/)
{
    auto file = App::project()->file<Gerber::File>(ui->cbxFile->currentData().toInt());
    createTPI(file->apertures());
}

void ThermalForm::createTPI(const Gerber::ApertureMap* value)
{
    m_sourcePreview.clear();
    if (value)
        m_apertures = *value;

    if (model)
        delete ui->treeView->model();

    model = new ThermalModel(ui->treeView);
    auto file = App::project()->file<Gerber::File>(ui->cbxFile->currentData().toInt());
    boardSide = file->side();
    model->appendRow(QIcon(), tr("All"), par);

    struct Worker {
        const Gerber::GraphicObject* go;
        ThermalNode* thermalNode;
        QString name;
        int strageIdx = -1;
    };

    QVector<Worker> map;
    auto creator = [this](Worker w) {
        static QMutex m;
        auto [go, thermalNode, name, strageIdx] = w;
        auto item = new ThermalPreviewItem(*go, tool, m_depth);
        connect(item, &ThermalPreviewItem::selectionChanged, this, &ThermalForm::setSelection);
        item->setToolTip(name);
        QMutexLocker lock(&m);
        m_sourcePreview.append(QSharedPointer<ThermalPreviewItem>(item));
        thermalNode->append(new ThermalNode(drawRegionIcon(*go), name, par, go->state().curPos(), item));
    };

    enum {
        Region = -2,
        Line
    };

    int ctr = 0;

    auto testArea = [this](const Paths& paths) {
        const double areaMax = ui->dsbxAreaMax->value() * uScale * uScale;
        const double areaMin = ui->dsbxAreaMin->value() * uScale * uScale;
        const double area = Area(paths);
        return areaMin <= area && area <= areaMax;
    };

    QMap<int, ThermalNode*> thermalNodes;
    if (ui->chbxAperture->isChecked()) {
        for (auto [dCode, aperture] : m_apertures) {
            if (aperture->isFlashed() && testArea(aperture->draw({}))) {
                thermalNodes[dCode] = model->appendRow(drawApertureIcon(aperture.data()), aperture->name(), par);
            }
        }
        for (auto [dCode, aperture] : m_apertures) {
            if (aperture->isFlashed()) {
                for (const Gerber::GraphicObject& go : *file) {
                    if (thermalNodes.contains(dCode)
                        && go.state().dCode() == Gerber::D03
                        && go.state().aperture() == dCode) {
                        map.append({ &go, thermalNodes[dCode], "", ctr++ });
                    }
                }
            }
        }
    }

    if (ui->chbxPath->isChecked()) {
        thermalNodes[Line] = model->appendRow(QIcon(), tr("Lines"), par);
        for (const Gerber::GraphicObject& go : *file) {
            if (go.state().type() == Gerber::Line
                && go.state().imgPolarity() == Gerber::Positive
                && (go.path().size() == 2 || (go.path().size() == 5 && go.path().first() == go.path().last()))
                && Length(go.path().first(), go.path().last()) * dScale * 0.3 < m_apertures[go.state().aperture()]->minSize()
                && testArea(go.paths())) {
                map.append({ &go, thermalNodes[Line], tr("Line"), ctr++ });
            }
        }
    }

    if (ui->chbxPour->isChecked()) {
        thermalNodes[Region] = model->appendRow(QIcon(), tr("Regions"), par);
        QVector<const Gerber::GraphicObject*> gos;
        for (const Gerber::GraphicObject& go : *file) {
            if (go.state().type() == Gerber::Region
                && go.state().imgPolarity() == Gerber::Positive
                && testArea(go.paths())) {
                gos.append(&go);
            }
        }
        std::sort(gos.begin(), gos.end(), [](const Gerber::GraphicObject* go1, const Gerber::GraphicObject* go2) {
            //            return go1->paths() < go2->paths();
            return go1->state().curPos() < go2->state().curPos();
        });
        for (auto go : gos) {
            map.append({ go, thermalNodes[Region], tr("Region"), ctr++ });
        }
    }

    storage.resize(ctr * sizeof(ThermalPreviewItem));

    for (int i = 0, c = QThread::idealThreadCount(); i < map.size(); i += c) {
        auto m(map.mid(i, c));
        QFuture<void> future = QtConcurrent::map(m, creator);
        future.waitForFinished();
    }

    for (QSharedPointer<ThermalPreviewItem> item : m_sourcePreview)
        App::scene()->addItem(item.data());

    //    updateCreateButton();

    ui->treeView->setModel(model);
    connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ThermalForm::onSelectionChanged);
    if (0 && qApp->applicationDirPath().contains("GERBER_X2/bin"))
        ui->treeView->expandAll();
}

void ThermalForm::onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    for (auto index : selected.indexes()) {
        auto* node = static_cast<ThermalNode*>(index.internalPointer());
        auto* item = node->item();
        if (item)
            item->setSelected(true);
        else {
            for (int i = 0; i < node->childCount(); ++i) {
                ui->treeView->selectionModel()->select(model->createIndex(i, 0, node->child(i)), QItemSelectionModel::Select | QItemSelectionModel::Rows);
            }
        }
    }
    for (auto index : deselected.indexes()) {
        auto* node = static_cast<ThermalNode*>(index.internalPointer());
        auto* item = node->item();
        if (item)
            item->setSelected(false);
        else {
            for (int i = 0; i < node->childCount(); ++i) {
                ui->treeView->selectionModel()->select(model->createIndex(i, 0, node->child(i)), QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
            }
        }
    }
}

void ThermalForm::setSelection(const QModelIndex& selected, const QModelIndex& deselected)
{
    if (selected.isValid())
        ui->treeView->selectionModel()->select(selected, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    if (deselected.isValid())
        ui->treeView->selectionModel()->select(deselected, QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
}

void ThermalForm::redraw()
{
    for (auto item : m_sourcePreview) {
        item->redraw();
    }
}

void ThermalForm::on_dsbxDepth_valueChanged(double arg1)
{
    m_depth = arg1;
    redraw();
}

void ThermalForm::editFile(GCode::File* /*file*/) { }

void ThermalForm::on_dsbxAreaMin_editingFinished()
{
    qDebug(Q_FUNC_INFO);
    if (lastMin != ui->dsbxAreaMin->value()) { // skip if dsbxAreaMin hasn't changed
        lastMin = ui->dsbxAreaMin->value();
        createTPI(nullptr);
    }
}

void ThermalForm::on_dsbxAreaMax_editingFinished()
{
    qDebug(Q_FUNC_INFO);
    if (lastMax != ui->dsbxAreaMax->value()) { // skip if dsbAreaMax hasn't changed
        lastMax = ui->dsbxAreaMax->value();
        createTPI(nullptr);
    }
}
