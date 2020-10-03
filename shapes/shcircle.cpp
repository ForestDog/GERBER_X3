// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "shcircle.h"
#include "math.h"
#include "scene.h"
#include "shhandler.h"
#include <QIcon>

#include "leakdetector.h"

namespace Shapes {

Circle::Circle(QPointF center, QPointF pt)
    : m_radius(QLineF(center, pt).length())
{
    m_paths.resize(1);
    handlers = { new Handler(this, Handler::Center), new Handler(this) };
    handlers[Center]->setPos(center);
    handlers[Point1]->setPos(pt);

    redraw();
    setFlags(ItemIsSelectable | ItemIsFocusable);
    setAcceptHoverEvents(true);
    setZValue(std::numeric_limits<double>::max());

    App::scene()->addItem(this);
    App::scene()->addItem(handlers[Center]);
    App::scene()->addItem(handlers[Point1]);
}

Circle::~Circle() { }

void Circle::redraw()
{
    m_radius = (QLineF(handlers[Center]->pos(), handlers[Point1]->pos()).length());
    const int intSteps = GlobalSettings::gbrGcCircleSegments(m_radius);
    const cInt radius = static_cast<cInt>(m_radius * uScale);
    const IntPoint center(toIntPoint(handlers[Center]->pos()));
    const double delta_angle = (2.0 * M_PI) / intSteps;
    Path& path = m_paths.first();
    path.clear();
    for (int i = 0; i < intSteps; i++) {
        const double theta = delta_angle * i;
        path.append(IntPoint(
            static_cast<cInt>(radius * cos(theta)) + center.X,
            static_cast<cInt>(radius * sin(theta)) + center.Y));
    }
    path.append(path.first());
    m_shape = QPainterPath();
    m_shape.addPolygon(toQPolygon(path));
    m_rect = m_shape.boundingRect();
    m_scale = std::numeric_limits<double>::max();
    setPos({ 1, 1 }); //костыли    //update();
    setPos({ 0, 0 });
}

QString Circle::name() const { return QObject::tr("Circle"); }

QIcon Circle::icon() const { return QIcon::fromTheme("draw-ellipse"); }

QPointF Circle::calcPos(Handler* sh) { return sh->pos(); }

void Circle::setPt(const QPointF& pt)
{
    if (handlers[Point1]->pos() == pt)
        return;
    handlers[Point1]->setPos(pt);
    redraw();
}

double Circle::radius() const
{
    return m_radius;
}

void Circle::setRadius(double radius)
{
    if (!qFuzzyCompare(m_radius, radius))
        return;
    m_radius = radius;
    redraw();
}
}
