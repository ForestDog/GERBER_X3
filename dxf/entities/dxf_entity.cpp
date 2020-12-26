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
#include "dxf_entity.h"
#include "dxf_file.h"
#include "tables/dxf_layer.h"
#include <QMetaEnum>

namespace Dxf {
Entity::Entity(SectionParser* sp)
    : sp(sp)
{
}

Entity::Type Entity::TypeVal(const QString& key)
{
    return static_cast<Type>(staticMetaObject.enumerator(0).keyToValue(key.toLocal8Bit().toUpper().data()));
}

QString Entity::typeName(int key)
{
    return staticMetaObject.enumerator(0).valueToKey(key);
}

QString Entity::name() const
{
    return staticMetaObject.enumerator(0).valueToKey(type());
}

void Entity::parseEntity(CodeData& code)
{
    data.push_back(code);
    switch (code.code()) {
    case LayerName:
        layerName = code.string();
        break;
    case Handle:
        handle = code.string();
        break;
    default:
        break;
    }
}

QColor Entity::color() const
{
    if (auto layer = sp->file->layer(layerName); layer != nullptr) {
        QColor c(dxfColors[layer->colorNumber()]);
        c.setAlpha(200);
        return c;
    }

    return QColor(255, 0, 255, 100);
}

void Entity::attachToLayer(GraphicObject&& go) const
{
    if (sp == nullptr)
        throw QString("SectionParser is null!");
    else if (sp->file == nullptr)
        throw QString("File in SectionParser is null!");
    else if (sp->file->layer(layerName))
        sp->file->layer(layerName)->addGraphicObject(std::move(go));
    else {
        throw QString("Layer '%1' not found in file!").arg(layerName);
    }
}

QPointF polar(QPointF p, float angle, float distance)
{
    // Returns the point at a specified `angle` and `distance` from point `p`.
    return p + QPointF(cos(angle) * distance, sin(angle) * distance);
}

double angle(QPointF p1, QPointF p2)
{
    // Returns angle a line defined by two endpoints and x-axis in radians.
    p2 -= p1;
    return atan2(p2.y(), p2.x());
}

double signedBulgeRadius(QPointF start_point, QPointF end_point, double bulge)
{
    return QLineF(start_point, end_point).length() * (1.0 + (bulge * bulge)) / 4.0 / bulge;
}

std::tuple<QPointF, double, double, double> bulgeToArc(QPointF start_point, QPointF end_point, float bulge)
{
    /*
    Returns arc parameters from bulge parameters.
    Based on Bulge to Arc by `Lee Mac`_.
    Args:
        start_point: start vertex as :class:`Vec2` compatible object
        end_point: end vertex as :class:`Vec2` compatible object
        bulge: bulge value
    Returns:
        Tuple: (center, start_angle, end_angle, radius)
    */
    double r = signedBulgeRadius(start_point, end_point, bulge);
    double a = angle(start_point, end_point) + (M_PI / 2.0 - atan(bulge) * 2.0);
    QPointF c = polar(start_point, a, r);
    if (bulge < 0.0)
        return { c, angle(c, end_point), angle(c, start_point), abs(r) };
    else
        return { c, angle(c, start_point), angle(c, end_point), abs(r) };
}

}
