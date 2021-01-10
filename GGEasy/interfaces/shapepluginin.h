/*******************************************************************************
*                                                                              *
* Author    :  Damir Bakiev                                                    *
* Version   :  na                                                              *
* Date      :  01 February 2020                                                *
* Website   :  na                                                              *
* Copyright :  Damir Bakiev 2016-2021                                          *
*                                                                              *
* License:                                                                     *
* Use, modification & distribution is subject to Boost Software License Ver 1. *
* http://www.boost.org/LICENSE_1_0.txt                                         *
*                                                                              *
*******************************************************************************/
#pragma once

#include <shape.h>

#include <QObject>
#include <app.h>

class ShapePluginInterface {
public:
    explicit ShapePluginInterface() { }
    virtual ~ShapePluginInterface() { }
    virtual QObject* getObject() = 0;
    virtual int type() const = 0;
    virtual Shapes::Shape* createShape(const QPointF& point) = 0;
    virtual bool addShapePoint(const QPointF& point) = 0;
    virtual void updateShape(const QPointF& point) = 0;
    virtual void finalizeShape() = 0;

    //    [[nodiscard]] virtual DrillPreviewGiMap createDrillPreviewGi(
    //        [[maybe_unused]] FileInterface* file,
    //        [[maybe_unused]] mvector<Row>& data) { return {}; };
    //    [[nodiscard]] virtual ThermalPreviewGiVec createThermalPreviewGi(
    //        [[maybe_unused]] FileInterface* file,
    //        [[maybe_unused]] const ThParam2& param,
    //        [[maybe_unused]] Tool& tool) { return {}; };
    //    [[nodiscard]] virtual NodeInterface* createNode(FileInterface* file) = 0;
    //    [[nodiscard]] virtual SettingsTab createSettingsTab([[maybe_unused]] QWidget* parent) { return { nullptr, "" }; };
    //    [[nodiscard]] virtual std::shared_ptr<FileInterface> createFile() = 0;
    [[nodiscard]] virtual QJsonObject info() const = 0;
    //    virtual void addToDrillForm([[maybe_unused]] FileInterface* file, [[maybe_unused]] QComboBox* cbx) {};
    //    virtual void createMainMenu([[maybe_unused]] QMenu& menu, [[maybe_unused]] FileTreeView* tv)
    //    {
    //        menu.addAction(QIcon::fromTheme("document-close"), QObject::tr("&Close All Files"), [tv] {
    //            if (QMessageBox::question(tv, "", QObject::tr("Really?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    //                tv->closeFiles();
    //        });
    //    };
    virtual void setupInterface(App* a) = 0;
    //    virtual void updateFileModel([[maybe_unused]] FileInterface* file) {};

    //    // signals:
    //    virtual void fileError(const QString& fileName, const QString& error) = 0;
    //    virtual void fileWarning([[maybe_unused]] const QString& fileName, [[maybe_unused]] const QString& warning) {};
    //    virtual void fileProgress(const QString& fileName, int max, int value) = 0;
    //    virtual void fileReady(FileInterface* file) = 0;

    // slots:
    //    virtual FileInterface* parseFile(const QString& fileName, int type) = 0;

protected:
    App app;
    enum { IconSize = 24 };
};

#define ShapePlugin_iid "ru.xray3d.XrSoft.GGEasy.ShapePluginInterface"

Q_DECLARE_INTERFACE(ShapePluginInterface, ShapePlugin_iid)
