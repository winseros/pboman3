#include "treenode.h"
#include <QRegularExpression>
#include <algorithm>

constexpr const char* rePath = "\\\\|/";
constexpr const char* pathSep = "\\";

namespace pboman3 {
    TreeNode::TreeNode(QString title, TreeNodeType nodeType, const TreeNode* parent, const PboEntry* entry = nullptr)
        : entry(entry),
          title_(std::move(title)),
          nodeType_(nodeType),
          parent_(parent),
          expanded_(false),
          isPendingToBeRemoved_(false) {
        setParent(parent);
    }

    TreeNode::TreeNode(QString containerName, const TreeNode* parent)
        : entry(nullptr),
          title_(std::move(containerName)),
          nodeType_(TreeNodeType::Container),
          parent_(parent),
          expanded_(false),
          isPendingToBeRemoved_(false) {
    }

    TreeNode::~TreeNode() {
        qDeleteAll(children_);
    }

    void TreeNode::addEntry(const PboEntry* pboEntry) {
        const QList<QString> segments = getChildPathSegments(pboEntry);
        const QString& childName = segments.first();
        if (segments.length() == 1) {
            insertSorted(new TreeNode(childName, TreeNodeType::File, this, pboEntry));
        } else {
            TreeNode* child = getOrCreateChild(childName);
            child->addEntry(pboEntry);
        }
    }

    void TreeNode::scheduleRemove(const PboEntry* pboEntry, bool schedule) {
        if (nodeType_ == TreeNodeType::Root) {
            children_.at(0)->scheduleRemove(pboEntry, schedule);
        } else if (nodeType_ == TreeNodeType::File) {
            assert(entry == pboEntry);
            isPendingToBeRemoved_ = schedule;
        } else {
            const QList<QString> segments = getChildPathSegments(pboEntry);
            const QString& childName = segments.first();

            bool set = false;
            int childrenPending = 0;
            for (TreeNode* node : children_) {
                if (!set && node->title() == childName) {
                    node->scheduleRemove(pboEntry, schedule);
                    set = true;
                }
                if (node->isPendingToBeRemoved() == schedule) childrenPending++;
            }

            if (nodeType_ == TreeNodeType::Dir)
                isPendingToBeRemoved_ = childrenPending == childCount();

            assert(set && "Could not set the child deletion pending status");
        }
    }

    void TreeNode::setParent(const TreeNode* parent) {
        path_.clear();
        if (parent) {
            parent_ = parent;
            if (parent->path().isEmpty()) {
                path_.append(title_);
            } else {
                path_.append(parent->path()).append(pathSep).append(title_);
            }
        }
    }

    QList<QString> TreeNode::getChildPathSegments(const PboEntry* pboEntry) const {
        const QRegularExpression reg(rePath, QRegularExpression::PatternOption::DontCaptureOption);
        QList<QString> segments = pboEntry->fileName
                                          .right(pboEntry->fileName.length() - path().length())
                                          .split(reg, Qt::SplitBehaviorFlags::SkipEmptyParts);
        return segments;
    }

    TreeNode* TreeNode::getOrCreateChild(const QString& childName) {
        for (TreeNode* node : children_) {
            if (node->title() == childName) {
                return node;
            }
        }
        auto* child = new TreeNode(std::move(childName), TreeNodeType::Dir, this);
        insertSorted(child);
        return child;
    }

    void TreeNode::insertSorted(TreeNode* node) {
        auto index = -1;
        for (int i = 0; i < children_.count(); i++) {
            if (*node < *children_[i]) {
                index = i;
                break;
            }
        }
        if (index >= 0) {
            children_.insert(index, node);
        } else {
            children_.append(node);
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

    int TreeNode::row() const {
        return parent_
                   ? static_cast<int>(parent_->children_.indexOf(this))
                   : 1;
    }

    int TreeNode::childCount() const {
        return static_cast<int>(children_.length());
    }

    const TreeNode* TreeNode::child(int index) const {
        return children_[index];
    }

    QIcon TreeNode::icon() const {
        return QIcon(nodeType_ == TreeNodeType::File
                         ? ":ifile.png"
                         : expanded_
                         ? ":ifolderopened.png"
                         : ":ifolderclosed.png");
    }

    void TreeNode::expand(bool expand) {
        expanded_ = expand;
    }

    bool TreeNode::isPendingToBeRemoved() const {
        return isPendingToBeRemoved_;
    }

    void TreeNode::collectEntries(QMap<const QString, const PboEntry*>& collection) const {
        if (nodeType_ == TreeNodeType::Dir) {
            for (TreeNode* child : children_) {
                child->collectEntries(collection);
            }
        } else if (nodeType_ == TreeNodeType::File) {
            assert(entry);
            collection.insert(entry->fileName, entry);
        } else {
            assert(false);
        }
    }

    bool operator<(const TreeNode& one, const TreeNode& two) {
        if (one.nodeType_ == two.nodeType_) {
            return one.title_.compare(two.title_, Qt::CaseInsensitive) < 0;
        }
        return one.nodeType_ < two.nodeType_;
    }

    RootNode::RootNode(QString fileName)
        : TreeNode("", TreeNodeType::Root, nullptr, nullptr) {
        children_.append(new TreeNode(std::move(fileName), this));
    }

    void RootNode::addEntry(const PboEntry* pboEntry) {
        children_.first()->addEntry(pboEntry);
    }
}
