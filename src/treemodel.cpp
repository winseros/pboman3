#include "treemodel.h"

namespace pboman3 {
    TreeModel::TreeModel(const PboModel* model)
            : model_(model) {

    }

    QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const {
        return QModelIndex();
    }

    QModelIndex TreeModel::parent(const QModelIndex& child) const {
        return QModelIndex();
    }

    int TreeModel::rowCount(const QModelIndex& parent) const {
        return 0;
    }

    int TreeModel::columnCount(const QModelIndex& parent) const {
        return 0;
    }

    QVariant TreeModel::data(const QModelIndex& index, int role) const {
        return QVariant();
    }
}