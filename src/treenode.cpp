#include <QRegularExpression>
#include "treenode.h"

constexpr const char* RE_PATH = "\\\\|/";
constexpr const char* PATH_SEP = "\\";

namespace pboman3 {
    TreeNode::TreeNode(QString title, TreeNodeType nodeType, const TreeNode* parent) :
            title_(std::move(title)),
            nodeType_(nodeType),
            parent_(parent),
            expanded_(false) {
        setParent(parent);
    }

    TreeNode::TreeNode(QString containerName, const TreeNode* parent) :
            title_(std::move(containerName)),
            nodeType_(TreeNodeType::Container),
            parent_(parent),
            expanded_(false) {
    }

    TreeNode::~TreeNode() {
        qDeleteAll(children_);
    }

    void TreeNode::addEntry(const PboEntry* entry) {
        const QRegularExpression reg(RE_PATH, QRegularExpression::PatternOption::DontCaptureOption);
        const QList<QString> segments = entry->fileName
                .right(entry->fileName.length() - path().length())
                .split(reg, Qt::SplitBehaviorFlags::SkipEmptyParts);

        QString childName = segments.first();
        if (segments.length() == 1) {
            insertSorted(new TreeNode(std::move(childName), TreeNodeType::File, this));
        } else {
            TreeNode* child = getOrCreateChild(childName);
            child->addEntry(entry);
        }
    }

    void TreeNode::setParent(const TreeNode* parent) {
        path_.clear();
        if (parent) {
            parent_ = parent;
            if (parent->path().isEmpty()) {
                path_.append(title_);
            } else {
                path_.append(parent->path()).append(PATH_SEP).append(title_);
            }
        }
    }

    TreeNode* TreeNode::getOrCreateChild(QString& childName) {
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

    const TreeNode* TreeNode::child(int index) {
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

    bool operator<(const TreeNode& one, const TreeNode& two) {
        if (one.nodeType_ == two.nodeType_) {
            return one.title_.compare(two.title_, Qt::CaseInsensitive) < 0;
        }
        return one.nodeType_ < two.nodeType_;
    }

    RootNode::RootNode(QString fileName)
            : TreeNode("", TreeNodeType::Root, nullptr) {
        children_.append(new TreeNode(std::move(fileName), this));
    }

    void RootNode::addEntry(const PboEntry* entry) {
        children_.first()->addEntry(entry);
    }
}