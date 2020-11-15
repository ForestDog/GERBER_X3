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

#include "app.h"
#include "datastream.h"
#include "myclipper.h"
#include "splashscreen.h"
#include <QDateTime>
#include <QFileInfo>
#include <gi/itemgroup.h>

using namespace ClipperLib;

enum class FileType {
    Gerber,
    Excellon,
    GCode,
};

enum Side {
    NullSide = -1,
    Top,
    Bottom
};

class AbstractFile {
    //    friend class Project;
    friend QDataStream& operator<<(QDataStream& stream, const QSharedPointer<AbstractFile>& file);
    friend QDataStream& operator>>(QDataStream& stream, QSharedPointer<AbstractFile>& file);

    friend QDataStream& operator<<(QDataStream& stream, const AbstractFile& file)
    {
        stream << static_cast<int>(file.type());
        file.write(stream);
        stream << file.m_id;
        stream << file.m_lines;
        stream << file.m_name;
        stream << file.m_mergedPaths;
        stream << file.m_groupedPaths;
        stream << file.m_side;
        stream << file.m_color;
        stream << file.m_date;
        stream << file.itemGroup()->isVisible();
        return stream;
    }

    friend QDataStream& operator>>(QDataStream& stream, AbstractFile& file)
    {
        file.read(stream);
        stream >> file.m_id;
        stream >> file.m_lines;
        stream >> file.m_name;
        stream >> file.m_mergedPaths;
        stream >> file.m_groupedPaths;
        stream >> file.m_side;
        stream >> file.m_color;
        stream >> file.m_date;

        if (App::splashScreen())
            App::splashScreen()->showMessage(QObject::tr("              Preparing: ") + file.shortName() + "\n\n\n", Qt::AlignBottom | Qt::AlignLeft, Qt::white);

        file.createGi();
        bool fl;
        stream >> fl;
        file.itemGroup()->setVisible(fl);
        return stream;
    }

public:
    AbstractFile();
    virtual ~AbstractFile();

    QString shortName() const;
    QString name() const;
    void setFileName(const QString& name);

    virtual ItemGroup* itemGroup() const = 0;

    Paths mergedPaths() const;
    Pathss groupedPaths() const;

    QList<QString>& lines();

    enum Group {
        CopperGroup,
        CutoffGroup,
    };

    virtual FileType type() const = 0;
    virtual void createGi() = 0;
    //    virtual void selected() = 0;

    Side side() const;
    void setSide(Side side);

    const QColor& color() const;
    void setColor(const QColor& color);

    int id() const;
    void setId(int id);

protected:
    virtual void write(QDataStream& stream) const = 0;
    virtual void read(QDataStream& stream) = 0;

    int m_id = -1;
    virtual Paths merge() const = 0;

    QVector<ItemGroup*> m_itemGroup;
    QString m_name;
    QList<QString> m_lines;
    mutable Paths m_mergedPaths;
    Pathss m_groupedPaths;

    Side m_side = Top;
    QColor m_color;
    QDateTime m_date;

    //    void _write(QDataStream& stream) const;
    //    void _read(QDataStream& stream);
};

//Q_DECLARE_METATYPE(AbstractFile)
