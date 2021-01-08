// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
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
#include "filemodel.h"
#ifdef DXF_
#include "dxf_file.h"
#include "dxf_node.h"
#include "dxf_types.h"
#include "tables/dxf_layer.h"
#endif

#include "foldernode.h"

//#include "project.h"
#ifdef SH_
#include "shheaders.h"
#endif

#include "interfaces/file.h"
#include "interfaces/pluginfile.h"

#include "leakdetector.h"

#include <QTimer>

FileModel::FileModel(QObject* parent)
    : QAbstractItemModel(parent)
    , rootItem(new FolderNode("rootItem", -1))
    , mimeType(QStringLiteral("application/GCodeItem"))
{
    rootItem->push_back(new FolderNode(tr("Gerber Files"), int(FileType::Gerber)));
    rootItem->push_back(new FolderNode(tr("Excellon"), int(FileType::Excellon)));
    rootItem->push_back(new FolderNode(tr("Tool Paths"), int(FileType::GCode)));
    rootItem->push_back(new FolderNode(tr("Dxf Files"), int(FileType::Dxf)));
    rootItem->push_back(new FolderNode(tr("Shapes"), int(FileType::Shapes)));
    App::m_app->m_fileModel = this;
}

FileModel::~FileModel()
{
    delete rootItem;
    App::m_app->m_fileModel = nullptr;
}

void FileModel::addFile(FileInterface* file)
{
    if (!file)
        return;
    const int type = int(file->type());
    NodeInterface* item(rootItem->child(type));
    QModelIndex index = createIndex(0, 0, item);
    int rowCount = item->childCount();

    NodeInterface* newItem = nullptr;
    beginInsertRows(index, rowCount, rowCount);
    item->push_back(newItem = App::parserInterface(type)->createNode(file));
    endInsertRows();
    if (newItem == nullptr)
        return;
    QModelIndex selectIndex = createIndex(rowCount, 0, newItem);
    file->setFileIndex(selectIndex);
    App::parserInterface(type)->updateFileModel(file);
    emit select(selectIndex);
}

void FileModel::addShape(Shapes::Shape* sh)
{
    if (sh == nullptr)
        return;
#ifdef SH_
    NodeInterface* item(rootItem->child(static_cast<int>(FileModel::Shapes)));
    QModelIndex index = createIndex(0, 0, item);
    int rowCount = item->childCount();

    beginInsertRows(index, rowCount, rowCount);
    auto node = new Shapes::Node(sh->id());
    item->push_back(node);
    endInsertRows();
    QModelIndex selectIndex = createIndex(rowCount, 0, node);
    qDebug() << __FUNCTION__ << selectIndex;
    sh->setFileIndex(selectIndex);
    emit select(selectIndex);
    emit select(createIndex(rowCount, 0, node));
#endif
}

void FileModel::closeProject()
{
    NodeInterface* item;
    for (int i = 0; i < rootItem->childCount(); ++i) {
        item = rootItem->child(i);
        QModelIndex index = createIndex(i, 0, item);
        int rowCount = item->childCount();
        if (rowCount) {
            beginRemoveRows(index, 0, rowCount - 1);
            for (int j = 0; j < rowCount; ++j)
                item->remove(0);
            endRemoveRows();
        }
    }
}

QModelIndex FileModel::index(int row, int column, const QModelIndex& parent) const
{
    NodeInterface* childItem = getItem(parent)->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex FileModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();
    NodeInterface* parentItem = getItem(index)->parentItem();
    if (parentItem == rootItem)
        return QModelIndex();
    return createIndex(parentItem->row(), 0, parentItem);
}

QVariant FileModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    NodeInterface* item = getItem(index);

    return item->data(index, role);
}

bool FileModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool ok = getItem(index)->setData(index, value, role);
    //    if (ok)
    //        App::project()->setChanged();
    return ok;
}

QVariant FileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole || role == Qt::ToolTipRole) && orientation == Qt::Horizontal)
        switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Side");
        case 2:
            return tr("Type");
        default:
            return QString("");
        }
    return QVariant();
}

Qt::ItemFlags FileModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return getItem(index)->flags(index);
}

bool FileModel::removeRows(int row, int count, const QModelIndex& parent)
{
    NodeInterface* item = nullptr;
    if (parent.isValid())
        item = static_cast<NodeInterface*>(parent.internalPointer());
    else
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    while (count--)
        item->remove(row);
    endRemoveRows();
    resetInternalData();
    return true;
}

int FileModel::columnCount(const QModelIndex& /*parent*/) const
{
    return int(NodeInterface::Column::Count);
}

int FileModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return 0;
    return getItem(parent)->childCount();
}

//QStringList FileModel::mimeTypes() const
//{
//    QStringList types;
//    types << mimeType;
//    return types;
//}

//QMimeData* FileModel::mimeData(const QModelIndexList& indexes) const
//{
//    //    QMimeData* mimeData = new QMimeData();
//    //    QByteArray encodedData;
//    //    int noCopy = -1;
//    //    for (const QModelIndex& index : indexes) {
//    //        if (noCopy != index.row()) {
//    //            noCopy = index.row();
//    //            ToolItem* item = static_cast<ToolItem*>(index.parent().internalPointer());
//    //            if (!item)
//    //                item = rootItem;
//    //            if (index.isValid()) {
//    //                encodedData.push_back(tr("%1,%2").arg(index.row()).arg((quint64)item /*index.internalPointer()*/).toLocal8Bit());
//    //                encodedData.push_back("|");
//    //            }
//    //        }
//    //    }
//    //    mimeData->setData(myModelMimeType(), encodedData);
//    //    return mimeData;
//    QMimeData* mimeData = new QMimeData();
//    QByteArray encodedData;
//    int noCopy = -1;
//    for (const QModelIndex& index : indexes) {
//        if (noCopy != index.row()) {
//            noCopy = index.row();
//            if (index.isValid()) {
//                encodedData.push_back(QString().setNum((quint64)index.internalPointer()).toLocal8Bit());
//                encodedData.push_back("|");
//            }
//        }
//    }
//    mimeData->setData(mimeType, encodedData);
//    return mimeData;
//}

//bool FileModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
//{
//    return false;
//    //    if (action == Qt::IgnoreAction)
//    //        return true;

//    //    if (!data->hasFormat(mimeType))
//    //        return false;

//    //    if (column > 0)
//    //        return false;

//    //    int beginRow;

//    //    if (row != -1)
//    //        beginRow = row;
//    //    else if (parent.isValid())
//    //        beginRow = parent.row();
//    //    else
//    //        beginRow = rowCount(QModelIndex());

//    //    QString encodedData = data->data(mimeType);
//    //    QList<QString> list = encodedData.split('|', QString::SkipEmptyParts);

//    //    //    for (QString& item : list) {
//    //    //        QList<QString> d = item.split(',', QString::SkipEmptyParts);
//    //    //        if (d.size() < 2)
//    //    //            return false;
//    //    //        int srcRow = d.at(0).toInt();
//    //    //        ToolItem* ti = reinterpret_cast<ToolItem*>(d.at(1).toLongLong());
//    //    //        QModelIndex index = createIndex(srcRow, 0, ti);
//    //    //        moveRows(index, srcRow, 1, parent, parent.row() > -1 ? parent.row() : 0);
//    //    //    }

//    //    for (QString& item : list) {
//    //        NodeInterface* copyItem = reinterpret_cast<NodeInterface*>(item.toLongLong());
//    //        NodeInterface* parentItem = static_cast<NodeInterface*>(parent.internalPointer());
//    //        if (copyItem) {
//    //            if (!parentItem)
//    //                parentItem = rootItem;
//    //            insertRows(beginRow, list.size(), parent);
//    //            if (parentItem->childCount() > beginRow)
//    //                parentItem->setChild(beginRow, new NodeInterface(*copyItem));
//    //            else
//    //                parentItem->setChild(parentItem->childCount() - 1, new NodeInterface(*copyItem));
//    //        }
//    //        ++beginRow;
//    //    }
//    //    return true;
//}

//Qt::DropActions FileModel::supportedDragActions() const
//{
//    return Qt::MoveAction | Qt::TargetMoveAction;
//}

//Qt::DropActions FileModel::supportedDropActions() const
//{
//    return Qt::MoveAction | Qt::TargetMoveAction;
//}

//bool FileModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
//{
//    return false;
//    //    beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);
//    //    NodeInterface* srcItem = static_cast<NodeInterface*>(sourceParent.internalPointer());
//    //    NodeInterface* dstItem = static_cast<NodeInterface*>(destinationParent.internalPointer());
//    //    if (!srcItem)
//    //        srcItem = rootItem;
//    //    if (!dstItem)
//    //        dstItem = rootItem;
//    //    for (int r = 0; r < count; ++r) {
//    //        dstItem->insertChild(destinationChild + r, srcItem->takeChild(sourceRow));
//    //    }
//    //    endMoveRows();
//    //    return true;
//}

NodeInterface* FileModel::getItem(const QModelIndex& index) const
{
    if (index.isValid()) {
        auto* item = static_cast<NodeInterface*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}
