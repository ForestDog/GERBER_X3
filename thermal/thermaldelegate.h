#pragma once

#include <QStyledItemDelegate>

class ThermalDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    ThermalDelegate(QObject* parent = nullptr);
    ~ThermalDelegate() override = default;

public:
    // QAbstractItemDelegate interface
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

    void emitCommitData();
};
