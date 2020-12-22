#pragma once
#include "dxf_entity.h"
namespace Dxf {
struct ELLIPSE final : Entity {
    ELLIPSE(SectionParser* sp);

    // Entity interface
public:
    void draw(const INSERT_ET* const i) const override;
    void parse(CodeData& code) override;
    Type type() const override { return Entity::ELLIPSE; }

    enum VarType {
        SubclassMarker = 100, //	100	Маркер подкласса (AcDbEllipse)

        CenterPointX = 10, //	10	Центральная точка (в МСК)
        CenterPointY = 20, //		Файл DXF: значение X; приложение: 3D-точка
        CenterPointZ = 30, //	20, 30	Файл DXF: значения Y и Z для центральной точки (в МСК)

        EndpointOfMajorAxisX = 11, //	11	Конечная точка главной оси относительно центральной точки (в МСК)
        EndpointOfMajorAxisY = 21, //		Файл DXF: значение X; приложение: 3D-точка
        EndpointOfMajorAxisZ = 31, //	21, 31	Файл DXF: значения Y и Z для конечной точки главной оси относительно центральной точки (в МСК)

        ExtrusionDirectionX = 210, //	210	Направление выдавливания (необязательно; значение по умолчанию = 0, 0, 1)
        ExtrusionDirectionY = 220, //		Файл DXF: значение X; приложение: 3D-вектор
        ExtrusionDirectionZ = 230, //	220, 230	Файл DXF: значения Y и Z для направления выдавливания (необязательно)

        RatioOfMinorAxisToMajorAxis = 40, //	40	Соотношение малой и главной осей

        StartParameter = 41, //	41	Начальный параметр (значение для полного эллипса — 0,0)
        EndParameter = 42, //	42	Конечный параметр (значение для полного эллипса — 2 пи)
    };

    double startParameter = 0.0;
    double endParameter = 0.0;
    double ratioOfMinorAxisToMajorAxis = 0.0;
    QPointF CenterPoint;
    QPointF EndpointOfMajorAxis;
};
}
