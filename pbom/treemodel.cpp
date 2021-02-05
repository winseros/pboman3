#include "treemodel.h"
#include <QFont>

namespace pboman3 {
    TreeModel::TreeModel(const PboModel* model) {
        connect(model, &PboModel::onEvent, this, &TreeModel::onModelEvent);

        pathSep_.setPattern("\\\\|/");
        pathSep_.setPatternOptions(QRegularExpression::PatternOption::DontCaptureOption);
    }

    QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const {
        if (!hasIndex(row, column, parent) || !root_)
            return QModelIndex();

        TreeNode* parentNode = parent.isValid()
                                   ? static_cast<TreeNode*>(parent.internalPointer())
                                   : root_.get();

        const TreeNode* childNode = parentNode->child(row);
        return createIndex(row, column, childNode);
    }

    QModelIndex TreeModel::parent(const QModelIndex& child) const {
        if (!child.isValid() || !root_)
            return QModelIndex();

        auto* childNode = static_cast<TreeNode*>(child.internalPointer());
        const TreeNode* parentNode = childNode->parent();
        return parentNode
                   ? createIndex(parentNode->row(), 0, parentNode)
                   : QModelIndex();
    }

    int TreeModel::rowCount(const QModelIndex& parent) const {
        if (!root_)
            return 0;

        auto* parentNode = parent.isValid()
                               ? static_cast<TreeNode*>(parent.internalPointer())
                               : root_.get();

        return parentNode->childCount();
    }

    int TreeModel::columnCount(const QModelIndex& parent) const {
        return 1;
    }

    QVariant TreeModel::data(const QModelIndex& index, int role) const {
        if (!root_)
            return QVariant();

        auto* node = index.isValid()
                         ? static_cast<TreeNode*>(index.internalPointer())
                         : root_.get();

        switch (role) {
        case Qt::DisplayRole:
            return QVariant(node->title());
        case Qt::DecorationRole:
            return node->icon();
        case Qt::FontRole: {
            QFont f;
            f.setStrikeOut(node->isPendingToBeRemoved());
            return f;
        }
        default:
            return QVariant();
        }
    }

    Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const {
        if (!index.isValid() || !root_)
            return Qt::NoItemFlags;
        return QAbstractItemModel::flags(index);
    }

    void TreeModel::onModelEvent(const PboModelEvent* event) {
        if (const auto* evt1 = dynamic_cast<const PboEntryCreatedEvent*>(event)) {
            root_->addEntry(evt1->entry);
        } else if (const auto* evt2 = dynamic_cast<const PboEntryDeleteScheduledEvent*>(event)) {
            root_->scheduleRemove(evt2->entry);
            emit dataChanged(QModelIndex(), QModelIndex());
        } else if (const auto* evt3 = dynamic_cast<const PboEntryDeleteCanceledEvent*>(event)) {
            root_->scheduleRemove(evt2->entry, false);
            emit dataChanged(QModelIndex(), QModelIndex());
        } else if (const auto* evt4 = dynamic_cast<const PboEntryDeleteCompleteEvent*>(event)) {

        } else if (const auto* evt5 = dynamic_cast<const PboLoadBeginEvent*>(event)) {
            const int lastSep = static_cast<int>(evt5->path.lastIndexOf(pathSep_)) + 1;
            auto fileName = evt5->path.right(evt5->path.length() - lastSep);
            root_ = QSharedPointer<RootNode>(new RootNode(move(fileName)));
            beginResetModel();
        } else if (dynamic_cast<const PboLoadCompleteEvent*>(event)) {
            endResetModel();
        } else if (dynamic_cast<const PboLoadFailedEvent*>(event)) {
            root_ = nullptr;
            endResetModel();
        }
    }

    void TreeModel::viewExpanded(const QModelIndex& index) const {
        if (root_) {
            auto* node = static_cast<TreeNode*>(index.internalPointer());
            node->expand(true);
        }
    }

    void TreeModel::viewCollapsed(const QModelIndex& index) const {
        if (root_) {
            auto* node = static_cast<TreeNode*>(index.internalPointer());
            node->expand(false);
        }
    }
}
