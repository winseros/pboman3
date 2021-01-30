#include <QRegularExpression>
#include "treemodel.h"

#define RE_PATH "\\\\|/"
#define PATH_SEP "\\"

namespace pboman3 {
    TreeNode::TreeNode(QString title, TreeNodeType nodeType) :
            title_(std::move(title)),
            nodeType_(nodeType),
            parent_(nullptr) {
    }

    TreeNode::~TreeNode() {
        qDeleteAll(children_);
    }

    void TreeNode::addEntry(const PboEntry* entry) {
        QRegularExpression reg(RE_PATH, QRegularExpression::PatternOption::DontCaptureOption);
        const QList<QString> segments = entry->fileName
                .right(entry->fileName.length() - path().length())
                .split(reg, Qt::SplitBehaviorFlags::SkipEmptyParts);

        QString childName = segments.first();
        if (segments.length() == 1) {
            children_.push_back(new TreeNode(std::move(childName), TreeNodeType::File));
        } else {
            TreeNode* child = getOrCreateChild(childName);
            child->addEntry(entry);
        }
    }

    void TreeNode::setParent(const TreeNode* parent) {
        path_.clear();
        if (parent) {
            if (parent->path().isEmpty()) {
                path_.append(title_);
            } else {
                path_.append(parent->path()).append(PATH_SEP).append(title_);
            }
        }
    }

    const TreeNode* TreeNode::parent() const {
        return parent_;
    }

    const QString& TreeNode::path() const {
        return path_;
    }

    const QString& TreeNode::title() const {
        return title_;
    }

    TreeNode* TreeNode::getOrCreateChild(QString& childName) {
        for (TreeNode* node : children_) {
            if (node->title() == childName) {
                return node;
            }
        }

        auto* child = new TreeNode(std::move(childName), TreeNodeType::Dir);
        child->setParent(this);
        children_.push_back(child);
        return child;
    }

    int TreeNode::row() const {
        return parent_->children_.indexOf(this);
    }

    int TreeNode::childCount() const {
        return children_.length();
    }

    const TreeNode* TreeNode::child(int index) {
        return children_[index];
    }

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
        if (parent.column() > 0 || !root_)
            return 0;

        auto* parentNode = parent.isValid()
                           ? static_cast<TreeNode*> (parent.internalPointer())
                           : root_.get();

        return parentNode->childCount();
    }

    int TreeModel::columnCount(const QModelIndex& parent) const {
        return 1;
    }

    QVariant TreeModel::data(const QModelIndex& index, int role) const {
        if (!root_ || role != Qt::DisplayRole)
            return QVariant();

        auto* node = index.isValid()
                     ? static_cast<TreeNode*>(index.internalPointer())
                     : root_.get();

        return node->title();
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
            int lastSep = evt->path.lastIndexOf(pathSep_) + 1;
            QString fileName = evt->path.right(evt->path.length() - lastSep);
            root_ = make_unique<TreeNode>(fileName, TreeNodeType::Container);
            beginResetModel();
        } else if (event->eventType == PboLoadCompleteEvent::eventType) {
            endResetModel();
        } else if (event->eventType == PboLoadFailedEvent::eventType) {
            root_ = nullptr;
            endResetModel();
        }
    }
}