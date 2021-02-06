#pragma once

#include <QRegularExpression>
#include <QSharedPointer>
#include "treenode.h"
#include "model/pbomodel.h"

namespace pboman3 {
    class TreeModel : public QAbstractItemModel {
    public:
        TreeModel(const PboModel* model);

        QModelIndex index(int row, int column, const QModelIndex& parent) const override;

        QModelIndex parent(const QModelIndex& child) const override;

        int rowCount(const QModelIndex& parent) const override;

        int columnCount(const QModelIndex& parent) const override;

        QVariant data(const QModelIndex& index, int role) const override;

        Qt::ItemFlags flags(const QModelIndex& index) const override;

    private:
        QRegularExpression pathSep_;
        QSharedPointer<RootNode> root_;

    private slots:
        void onModelEvent(const PboModelEvent* event);
    };
}
