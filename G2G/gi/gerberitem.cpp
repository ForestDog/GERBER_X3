#include "gerberitem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <gbrfile.h>
#include <graphicsview.h>
#include <scene.h>

GerberItem::GerberItem(Paths& paths, Gerber::File* file)
    : GraphicsItem(file)
    , m_paths(paths)
{
    redraw();
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable, true);
}

GerberItem::~GerberItem() {}

QRectF GerberItem::boundingRect() const { return m_shape.boundingRect(); }

QPainterPath GerberItem::shape() const { return m_shape; }

void GerberItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* /*widget*/)
{
    if (Scene::drawPdf()) {
        painter->setBrush(Qt::black);
        painter->setPen(Qt::NoPen); //QPen(Qt::black, 0.0));
        painter->drawPath(m_shape);
        return;
    }

    if (m_penColor)
        m_pen.setColor(*m_penColor);
    if (m_brushColor)
        m_brush.setColor(*m_brushColor);

    if (Scene::drawPdf()) {
        painter->setBrush(m_brush.color());
        painter->setPen(Qt::NoPen);
        painter->drawPath(m_shape);
        return;
    }

    QColor brColor(m_brush.color());
    QColor pnColor(brColor);

    if (option->state & QStyle::State_Selected) {
        brColor.setAlpha(255);
        pnColor.setAlpha(255);
    }
    if (option->state & QStyle::State_MouseOver) {
        if (option->state & QStyle::State_Selected) {
            brColor = brColor.darker(120);
            pnColor = pnColor.darker(120);
        } else {
            brColor.setAlpha(200);
            pnColor.setAlpha(255);
        }
    }

    QBrush brush(brColor);
    QPen pen(pnColor, 0.0);

    painter->setBrush(brush);
    painter->setPen(m_file ? pen : m_pen);
    painter->drawPath(m_shape);
}

int GerberItem::type() const { return GiGerber; }

void GerberItem::redraw()
{
    m_shape = QPainterPath();
    for (Path path : m_paths) {
        path.append(path.first());
        m_shape.addPolygon(toQPolygon(path));
    }
    setPos({ 1, 1 }); // костыли
    setPos({ 0, 0 });
    //update();
}

Paths GerberItem::paths() const { return m_paths; }

Paths* GerberItem::rPaths() { return &m_paths; }
