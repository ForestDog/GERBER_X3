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
//#include "gctypes.h"
#include "abstractfile.h"
#include "gcutils.h"

class Project;
class PathItem;

namespace GCode {

class File : public AbstractFile, private GCUtils {
    friend class ::PathItem;
    friend class ::Project;

public:
    explicit File();
    explicit File(const Pathss& toolPathss, const GCodeParams& gcp, const Paths& pocketPaths = {});
    bool save(const QString& name);
    GCodeType gtype() const;
    FileType type() const override { return FileType::GCode; }
    ItemGroup* itemGroup() const override { return m_itemGroups.last(); }

private:
    ////////////////////////////////////////
    Paths m_pocketPaths; /////
    Pathss m_toolPathss; /////
    const GCodeParams m_gcp; ////

    QVector<QVector<QPolygonF>> normalizedPathss(const QPointF& offset);
    QVector<QPolygonF> normalizedPaths(const QPointF& offset, const Paths& paths_ = {});

    void initSave();
    void genGcodeAndTile();
    void addInfo(bool fl = false);
    void startPath(const QPointF& point);
    void endPath();
    void statFile();
    void endFile();

    void createGiDrill();
    void createGiPocket();
    void createGiProfile();
    void createGiRaster();
    void createGiLaser();
    //////////////
    void saveDrill(const QPointF& offset);

    void saveLaserPocket(const QPointF& offset);
    void saveMillingPocket(const QPointF& offset);

    void saveLaserProfile(const QPointF& offset);
    void saveMillingProfile(const QPointF& offset);

    void saveMillingRaster(const QPointF& offset);

    void saveLaserHLDI(const QPointF& offset);
    QVector<QSharedPointer<QColor>> debugColor;

    // AbstractFile interfaces
protected:
    virtual Paths merge() const override { return {}; }
    void write(QDataStream& stream) const override;
    void read(QDataStream& stream) override;

public:
    void createGi() override;

    QVector<QString> gCodeText() const;
    Tool getTool() const;
    const GCodeParams& gcp() const;
};
}
