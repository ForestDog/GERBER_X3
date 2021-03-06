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

#include "datastream.h"
#include <myclipper.h>

namespace Dxf {

class File;
struct Entity;

class GraphicObject {
    friend class File;
    friend class Plugin;

    friend QDataStream& operator<<(QDataStream& stream, const GraphicObject& go)
    {
        stream << go.m_path;
        stream << go.m_paths;

        stream << go.m_rotationAngle;
        stream << go.m_scaleX;
        stream << go.m_scaleY;
        stream << go.m_pos;

        return stream;
    }

    friend QDataStream& operator>>(QDataStream& stream, GraphicObject& go)
    {
        stream >> go.m_path;
        stream >> go.m_paths;

        stream >> go.m_rotationAngle;
        stream >> go.m_scaleX;
        stream >> go.m_scaleY;
        stream >> go.m_pos;

        return stream;
    }

    const File* const m_gFile = nullptr;
    const Entity* const m_entity = nullptr;
    Path m_path;
    Paths m_paths;

    double m_rotationAngle = 0.0;
    double m_scaleX = 0.0;
    double m_scaleY = 0.0;
    QPointF m_pos;

public:
    GraphicObject() { }
    GraphicObject(const File* file, const Entity* entity, const Path& path, const Paths& paths);

    void setRotation(double rotationAngle);
    inline double rotationAngle() const { return m_rotationAngle; }

    void setScale(double scaleX, double scaleY);
    inline std::tuple<double, double> scale() const { return { m_scaleX, m_scaleY }; }
    inline double scaleX() const { return m_scaleX; }
    inline double scaleY() const { return m_scaleY; }

    void setPos(QPointF pos);
    inline QPointF pos() const { return m_pos; }

    inline const File* file() const { return m_gFile; }
    inline const Entity* entity() const { return m_entity; }
    // const
    inline const Path& path() const { return m_path; }
    inline const Paths& paths() const { return m_paths; }
    // edit
    inline Path& path() { return m_path; }
    inline Paths& paths() { return m_paths; }
};

}
