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
#include <QApplication>
#include <QSharedMemory>

#include "leakdetector.h"

class Application : public QApplication {
    Q_OBJECT

public:
    Application(int& argc, char** argv)
        : QApplication(argc, argv, true)
    {
        _singular = new QSharedMemory("SharedMemoryForOnlyOneInstanceOfYourBeautifulApplication", this);
    }
    ~Application()
    {
        if (_singular->isAttached()) {
            _singular->detach();
        }
    }

    bool lock()
    {
        if (_singular->attach(QSharedMemory::ReadOnly)) {
            _singular->detach();
            return false;
        }
        if (_singular->create(1)) {
            return true;
        }

        return false;
    }

private:
    QSharedMemory* _singular; // shared memory !! SINGLE ACCESS
};
