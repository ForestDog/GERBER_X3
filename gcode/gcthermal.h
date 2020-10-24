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
#pragma once
#include "gccreator.h"

namespace Gerber {
class File;
}

namespace GCode {
class ThermalCreator : public Creator {
public:
    ThermalCreator();
    ~ThermalCreator() override = default;

private:
    void createThermal(Gerber::File* file, const Tool& tool, const double depth);

protected:
    void create() override; // Creator interface
    GCodeType type() override { return Thermal; }
};
}
