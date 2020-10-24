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
#include "formsutil/formsutil.h"

namespace Ui {
class ProfileForm;
}
class BridgeItem;

class ProfileForm : public FormsUtil {
    Q_OBJECT

public:
    explicit ProfileForm(QWidget* parent = nullptr);
    ~ProfileForm() override;

private slots:
    void on_pbAddBridge_clicked();
    void on_leName_textChanged(const QString& arg1);

private:
    Ui::ProfileForm* ui;
    double m_size = 0.0;
    double m_lenght = 0.0;
    void updateBridge();
    void updatePixmap();
    const QStringList names;
    const QStringList pixmaps;
    void rb_clicked();
    BridgeItem* brItem = nullptr;

protected:
    // QWidget interface
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    // FormsUtil interface
    void createFile() override;
    void updateName() override;

public:
    virtual void editFile(GCode::File* file) override;
};
