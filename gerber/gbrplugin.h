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

#include "gbrparser.h"

#include "interfaces/fileplugin.h"

#include <QObject>
#include <QStack>

namespace Gerber {

class Plugin : public QObject, public FilePluginInterface, Parser {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ParserInterface_iid FILE "gerber.json")
    Q_INTERFACES(FilePluginInterface)

public:
    Plugin(QObject* parent = nullptr);

    bool thisIsIt(const QString& fileName) override;
    QObject* getObject() override;
    int type() const override;
    NodeInterface* createNode(FileInterface* file) override;
    std::shared_ptr<FileInterface> createFile() override;
    void setupInterface(App* a) override;
    void createMainMenu(QMenu& menu, FileTreeView* tv) override;
    std::pair<SettingsTabInterface*, QString> createSettingsTab(QWidget* parent) override;
    void addToDrillForm(FileInterface* file, QComboBox* cbx) override;
    DrillPreviewGiMap createtDrillPreviewGi(FileInterface* file, std::vector<Row>& data) override;

public slots:
    FileInterface* parseFile(const QString& fileName, int type) override;

signals:
    void fileReady(FileInterface* file) override;
    void fileProgress(const QString& fileName, int max, int value) override;
    void fileError(const QString& fileName, const QString& error) override;

private:
    QIcon drawApertureIcon(Gerber::AbstractAperture* aperture);
};
}
