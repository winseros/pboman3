#include "treenode.h"
#include <QException>
#include <QRegularExpression>
#include <algorithm>

constexpr const char* rePath = "\\\\|/";
constexpr const char* pathSep = "\\";

namespace pboman3 {
    TreeNode::TreeNode(QString title, TreeNodeType nodeType, const QPointer<TreeNode>& parentNode,
                       const PboEntry* entry = nullptr)
        : QObject(),
          entry_(entry),
          title_(std::move(title)),
          nodeType_(nodeType),
          parentNode_(parentNode),
          expanded_(false),
          isPendingToBeRemoved_(false) {
        setParentNode(parentNode);
    }

    TreeNode::TreeNode(QString containerName, const QPointer<TreeNode>& parentNode)
        : QObject(),
          entry_(nullptr),
          title_(std::move(containerName)),
          nodeType_(TreeNodeType::Container),
          parentNode_(parentNode),
          expanded_(false),
          isPendingToBeRemoved_(false) {
    }

    bool operator<(const TreeNode& one, const TreeNode& two) {
        if (one.nodeType_ == two.nodeType_) {
            return one.title_.compare(two.title_, Qt::CaseInsensitive) < 0;
        }
        return one.nodeType_ < two.nodeType_;
    }

    void TreeNode::addEntry(const PboEntry* entry) {
        const QList<QString> segments = getPathComponents(entry);
        const QString& childName = segments.first();
        if (segments.length() == 1) {
            insertSorted(QSharedPointer<TreeNode>(new TreeNode(childName, TreeNodeType::File, this, entry)));
        } else {
            const QSharedPointer<TreeNode> child = getOrCreateChild(childName);
            child->addEntry(entry);
        }
    }

    void TreeNode::scheduleRemove(const PboEntry* entry, bool schedule) {
        if (nodeType_ == TreeNodeType::Root) {
            childNodes_.at(0)->scheduleRemove(entry, schedule);
        } else if (nodeType_ == TreeNodeType::File) {
            assert(entry_ == entry);
            isPendingToBeRemoved_ = schedule;
        } else {
            const QString childName = getPathComponents(entry).first();
            bool set = false;
            int childrenPending = 0;
            for (const QSharedPointer<TreeNode>& node : childNodes_) {
                if (!set && node->title() == childName) {
                    node->scheduleRemove(entry, schedule);
                    set = true;
                }
                if (node->isPendingToBeRemoved_ == schedule) childrenPending++;
            }

            if (nodeType_ == TreeNodeType::Dir)
                isPendingToBeRemoved_ = childrenPending == childNodeCount();

            assert(set && "Could not set the child deletion pending status");
        }
    }

    QPointer<TreeNode> TreeNode::prepareRemove(const PboEntry* entry) const {
        if (nodeType_ == TreeNodeType::Root) {
            return childNodes_.at(0)->prepareRemove(entry);
        }
        QPointer<TreeNode> nodeToRemove = findFileNode(entry);
        while (nodeToRemove->parentNode_->nodeType_ == TreeNodeType::Dir && nodeToRemove->parentNode_->childNodeCount() == 1) {
            nodeToRemove = nodeToRemove->parentNode_;
        }

        return nodeToRemove;
    }

    void TreeNode::completeRemove(const PboEntry* entry) {
        if (nodeType_ == TreeNodeType::Root) {
            childNodes_.at(0)->completeRemove(entry);
        } else {
            const QString childName = getPathComponents(entry).first();
            auto node = childNodes_.begin();
            while (node != childNodes_.end()) {
                if ((*node)->title() == childName) {
                    if ((*node)->nodeType_ == TreeNodeType::File) {
                        node = childNodes_.erase(node);
                        return;
                    }
                    if ((*node)->nodeType_ == TreeNodeType::Dir) {
                        (*node)->completeRemove(entry);
                        if (!(*node)->childNodeCount()) {
                            node = childNodes_.erase(node);
                        }
                        return;
                    }
                }
                ++node;
            }

            assert(false && "Could not delete the children");
        }
    }

    void TreeNode::collectEntries(QMap<const QString, const PboEntry*>& collection) const {
        if (nodeType_ == TreeNodeType::Dir) {
            for (const QSharedPointer<TreeNode>& child : childNodes_) {
                child->collectEntries(collection);
            }
        } else if (nodeType_ == TreeNodeType::File) {
            assert(entry_);
            collection.insert(entry_->fileName, entry_);
        } else {
            assert(false);
        }
    }

    QPointer<TreeNode> TreeNode::parentNode() const {
        return parentNode_;
    }

    void TreeNode::setParentNode(const QPointer<TreeNode>& parentNode) {
        path_.clear();
        if (parentNode) {
            parentNode_ = parentNode;
            if (parentNode->path().isEmpty()) {
                path_.append(title_);
            } else {
                path_.append(parentNode->path()).append(pathSep).append(title_);
            }
        }
    }

    QSharedPointer<TreeNode> TreeNode::getOrCreateChild(const QString& childName) {
        for (const QSharedPointer<TreeNode>& node : childNodes_) {
            if (node->title() == childName) {
                return node;
            }
        }
        auto child = QSharedPointer<TreeNode>(new TreeNode(childName, TreeNodeType::Dir, this));
        insertSorted(child);
        return child;
    }

    QPointer<TreeNode> TreeNode::findFileNode(const PboEntry* entry) const {
        const QList<QString> segments = getPathComponents(entry);
        auto seg = segments.begin();
        auto node = childNodes_.begin();
        auto end = childNodes_.end();
        while (seg != segments.end() && node != end) {
            if ((*node)->title_ == *seg) {
                if ((*node)->nodeType_ == TreeNodeType::File) {
                    return QPointer<TreeNode>(node->get());
                }
                ++seg;
                node = (*node)->childNodes_.begin();
                end = (*node)->childNodes_.end();
            }
            else {
                ++node;
            }
        }
        assert(false && "Could not find child");
        throw QException();
    }

    QList<QString> TreeNode::getPathComponents(const PboEntry* entry) const {
        const QRegularExpression reg(rePath, QRegularExpression::PatternOption::DontCaptureOption);
        const QList<QString> segments = entry->fileName
            .right(entry->fileName.length() - path().length())
            .split(reg, Qt::SkipEmptyParts);
        return segments;
    }

    void TreeNode::insertSorted(const QSharedPointer<TreeNode>& node) {
        auto index = -1;
        int i = 0;
        auto it = childNodes_.begin();
        while (it != childNodes_.end()) {
            if (*node < **it) {
                index = i;
                break;
            }
            ++it;
            ++i;
        }

        if (index >= 0) {
            childNodes_.insert(index, node);
        } else {
            childNodes_.append(node);
        }
    }

    const PboEntry* TreeNode::entry() const {
        return entry_;
    }

    const QString& TreeNode::title() const {
        return title_;
    }

    const QString& TreeNode::path() const {
        return path_;
    }

    void TreeNode::expand(bool expand) {
        expanded_ = expand;
    }

    QIcon TreeNode::icon() const {
        return QIcon(nodeType_ == TreeNodeType::File
                         ? ":ifile.png"
                         : expanded_
                         ? ":ifolderopened.png"
                         : ":ifolderclosed.png");
    }

    QFont TreeNode::font() const {
        QFont font;
        font.setStrikeOut(isPendingToBeRemoved_);
        return font;
    }

    int TreeNode::row() const {
        return parentNode_
                   ? static_cast<int>(parentNode_->childNodes_.indexOf(this))
                   : 1;
    }

    int TreeNode::childNodeCount() const {
        return static_cast<int>(childNodes_.length());
    }

    QPointer<TreeNode> TreeNode::childNode(int index) const {
        return QPointer<TreeNode>(childNodes_[index].get());
    }

    RootNode::RootNode(QString fileName)
        : TreeNode("", TreeNodeType::Root, nullptr, nullptr) {
        childNodes_.append(QSharedPointer<TreeNode>(new TreeNode(std::move(fileName), this)));
    }

    void RootNode::addEntry(const PboEntry* pboEntry) {
        childNodes_.first()->addEntry(pboEntry);
    }
}
