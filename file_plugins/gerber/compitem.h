/*******************************************************************************
*                                                                              *
* Author    :  Damir Bakiev                                                    *
* Version   :  na                                                              *
* Date      :  14 January 2021                                                 *
* Website   :  na                                                              *
* Copyright :  Damir Bakiev 2016-2021                                          *
*                                                                              *
* License:                                                                     *
* Use, modification & distribution is subject to Boost Software License Ver 1. *
* http://www.boost.org/LICENSE_1_0.txt                                         *
*                                                                              *
*******************************************************************************/
#pragma once

#include "gbrcomponent.h"
#include <graphicsitem.h>

namespace Gerber {

class ComponentItem final : public GraphicsItem {
    const Gerber::Component& m_component;
    QVector<QRectF> pins;
    mutable QPainterPath pathRefDes;
    mutable QVector<QPair<QPainterPath, QPointF>> pathPins;
    mutable double m_scale = std::numeric_limits<double>::max();
    mutable QPointF pt;
    bool m_selected {};

public:
    ComponentItem(const Gerber::Component& component, FileInterface* file);

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    // GraphicsItem interface
    Paths paths() const override;
    void changeColor() override { }

    void setSelected(bool selected) { m_selected = selected; }
};

}
