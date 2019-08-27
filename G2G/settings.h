#ifndef SETTINGS_H
#define SETTINGS_H

#include <QColor>
#include <QPointF>

enum class Colors : unsigned {
    Background,
    Pin,
    CutArea,
    Grid1,
    Grid5,
    Grid10,
    Hole,
    Home,
    ToolPath,
    Zero,
    G0,
    Count
};

enum class HomePosition : unsigned {
    BottomLeft,
    BottomRight,
    TopLeft,
    TopRight,
};

class Settings {
public:
    Settings();

    static int circleSegments(double radius);
    static QColor& color(Colors id);
    static bool cleanPolygons();
    static bool skipDuplicates();

    static QString startGCode();
    static QString endGCode();
    static QString gCodeFormat();

    static QPointF homeOffset();
    static int homePos();

    static QPointF pinOffset();

    static bool gcinfo();
    static bool inch();
    static void setInch(bool val);

    static double gridStep(double scale);
    static bool smoothScSh();

protected:
    static QColor m_color[static_cast<int>(Colors::Count)];
    static int m_minCircleSegments;
    static double m_minCircleSegmentLength;
    static bool m_cleanPolygons;
    static bool m_skipDuplicates;
    static bool m_gcinfo;
    static QString m_startGCode;
    static QString m_endGCode;
    static QString m_GCode;

    static QPointF m_pinOffset;
    static QPointF m_homeOffset;
    static int m_homePos;
    static bool m_inch;
    static bool m_smoothScSh;
};

#endif // SETTINGS_H
