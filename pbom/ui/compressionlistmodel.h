#pragma once

#include <QAbstractItemModel>
#include "model/filesystemfiles.h"

namespace pboman3 {
    class CompressionListModel: public QAbstractItemModel {
    public:
        CompressionListModel(QObject* parent, const FilesystemFiles* files);

        int rowCount(const QModelIndex& parent) const override;

        int columnCount(const QModelIndex& parent) const override;

        QVariant data(const QModelIndex& index, int role) const override;

        bool setData(const QModelIndex& index, const QVariant& value, int role) override;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        Qt::ItemFlags flags(const QModelIndex& index) const override;

        QModelIndex index(int row, int column, const QModelIndex& parent) const override;

        QModelIndex parent(const QModelIndex& child) const override;
    private:
        const FilesystemFiles* files_;
    };
}
