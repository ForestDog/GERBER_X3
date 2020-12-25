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
#include "gbraperture.h"
#include "gbrcomponent.h"
#include "gbrtypes.h"

#include "abstractfile.h"
#include <QDebug>
#include <forward_list>
#include <gi/itemgroup.h>
namespace Gerber {

class File : public AbstractFile, public QVector<GraphicObject> {
    friend class Parser;

public:
    explicit File(const QString& name = "");
    ~File() override;

    enum Group {
        CopperGroup,
        CutoffGroup,
    };

    Format* format() { return &m_format; }
    Pathss& groupedPaths(Group group = CopperGroup, bool fl = false);
    bool flashedApertures() const;
    const ApertureMap* apertures() const;

    enum ItemsType {
        NullType = -1,
        Normal,
        ApPaths,
        Components,
    };
    void setItemType(ItemsType type);
    ItemsType itemsType() const { return m_itemsType; }

    FileType type() const override { return FileType::Gerber; }

    ItemGroup* itemGroup(ItemsType type) const;
    ItemGroup* itemGroup() const override;

    void addToScene() const;
    void setColor(const QColor& color);

protected:
    Paths merge() const override;

private:
    QList<Component> m_components;

    ApertureMap m_apertures;
    ItemsType m_itemsType = NullType;
    void grouping(PolyNode* node, Pathss* pathss, Group group);
    Format m_format;

    //Layer layer = Copper;
    //Miror miror = Vertical;
    //QPointf offset;

    QVector<int> rawIndex;
    std::forward_list<Path> checkList;

    // AbstractFile interface
protected:
    void write(QDataStream& stream) const override;
    void read(QDataStream& stream) override;

    // AbstractFile interface
public:
    void createGi() override;
    const QList<Component>& components() const;
};
}

Q_DECLARE_METATYPE(Gerber::File)
