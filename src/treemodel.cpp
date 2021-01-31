#include <QIcon>
#include "treemodel.h"

#define RE_PATH "\\\\|/"

namespace pboman3 {
    TreeModel::TreeModel(const PboModel* model)
            : model_(model) {
        connect(model, &PboModel::onEvent, this, &TreeModel::onModelEvent);

        pathSep_.setPattern(RE_PATH);
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
        if (!root_) return 0;

        auto* parentNode = parent.isValid()
                           ? static_cast<TreeNode*> (parent.internalPointer())
                           : root_.get();

        return parentNode->childCount();
    }

    int TreeModel::columnCount(const QModelIndex& parent) const {
        return 1;
    }

    QVariant TreeModel::data(const QModelIndex& index, int role) const {
        if (!root_ || (role != Qt::DisplayRole && role != Qt::DecorationRole))
            return QVariant();

        auto* node = index.isValid()
                     ? static_cast<TreeNode*>(index.internalPointer())
                     : root_.get();

        return role == Qt::DisplayRole
               ? QVariant(node->title())
               : node->icon();
    }

    Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const {
        if (!index.isValid() || !root_)
            return Qt::NoItemFlags;
        return QAbstractItemModel::flags(index);
    }

    void TreeModel::onModelEvent(const PboModelEvent* event) {
        if (event->eventType == PboEntryUpdatedEvent::eventType) {
            auto evt = dynamic_cast<const PboEntryUpdatedEvent*>(event);
            root_->addEntry(evt->entry);
        } else if (event->eventType == PboLoadBeginEvent::eventType) {
            auto evt = dynamic_cast<const PboLoadBeginEvent*>(event);
            int lastSep = static_cast<int>(evt->path.lastIndexOf(pathSep_)) + 1;
            QString fileName = evt->path.right(evt->path.length() - lastSep);
            root_ = make_unique<RootNode>(std::move(fileName));
            beginResetModel();
        } else if (event->eventType == PboLoadCompleteEvent::eventType) {
            endResetModel();
        } else if (event->eventType == PboLoadFailedEvent::eventType) {
            root_ = nullptr;
            endResetModel();
        }
    }

    void TreeModel::viewExpanded(const QModelIndex& index) {
        if (root_) {
//            setData(index, QIcon(":ifolderopened"), Qt::DecorationRole);
            auto* node = static_cast<TreeNode*>(index.internalPointer());
            node->expand(true);
        }
    }

    void TreeModel::viewCollapsed(const QModelIndex& index) {
        if (root_) {
//            setData(index, QIcon(":ifolderopened"), Qt::DecorationRole);
            auto* node = static_cast<TreeNode*>(index.internalPointer());
            node->expand(false);
        }
    }
}