#include "compressionlistmodel.h"

namespace pboman3 {
    CompressionListModel::CompressionListModel(QObject* parent, const FilesystemFiles* files)
        : QAbstractItemModel(parent),
          files_(files) {
    }

    int CompressionListModel::rowCount(const QModelIndex& parent) const {
        return parent.isValid() ? 0 : static_cast<int>(files_->length());
    }

    int CompressionListModel::columnCount(const QModelIndex& parent) const {
        return 2;
    }

    QVariant CompressionListModel::data(const QModelIndex& index, int role) const {
        if (index.isValid()) {
            if (index.column() == 0 && role == Qt::DisplayRole) {
                const auto* file = static_cast<FilesystemFile*>(index.internalPointer());
                return file->pboPath;
            }
            if (index.column() == 1 && role == Qt::CheckStateRole) {
                const auto* file = static_cast<FilesystemFile*>(index.internalPointer());
                return file->compress ? Qt::Checked : Qt::Unchecked;
            }
        }
        return QVariant();
    }

    bool CompressionListModel::setData(const QModelIndex& index, const QVariant& value, int role) {
        if (index.isValid() && index.column() == 1 && role == Qt::CheckStateRole) {
            auto* file = static_cast<FilesystemFile*>(index.internalPointer());
            file->compress = value == Qt::Checked;
        }
        return false;
    }

    QVariant CompressionListModel::headerData(int section, Qt::Orientation orientation, int role) const {
        if (role == Qt::DisplayRole) {
            return section == 0 ? "File" : "Compress";
        }
        return QVariant();
    }

    Qt::ItemFlags CompressionListModel::flags(const QModelIndex& index) const {
        Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable;

        if (index.column() == 1)
            flags |= Qt::ItemFlag::ItemIsUserCheckable;

        return flags;
    }

    QModelIndex CompressionListModel::index(int row, int column, const QModelIndex& parent) const {
        const FilesystemFile& file = (*files_)[row];
        return createIndex(row, column, &file);
    }

    QModelIndex CompressionListModel::parent(const QModelIndex& child) const {
        return QModelIndex();
    }
}
