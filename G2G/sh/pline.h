#pragma once

#include "shape.h"

namespace ShapePr {
class Pline final : public Shape {
public:
    Pline(QPointF pt1, QPointF pt2);
    Pline(QDataStream& stream);
    ~Pline();

    // QGraphicsItem interface
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    int type() const override { return GiShapeL; }

    // GraphicsItem interface
    void redraw() override;

    void setPt(const QPointF& pt);
    void addPt(const QPointF& pt);
    bool closed();
};
}
