#include <QRegularExpression>
#include "treenode.h"

#define RE_PATH "\\\\|/"
#define PATH_SEP "\\"

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
        QRegularExpression reg(RE_PATH, QRegularExpression::PatternOption::DontCaptureOption);
        const QList<QString> segments = entry->fileName
                .right(entry->fileName.length() - path().length())
                .split(reg, Qt::SplitBehaviorFlags::SkipEmptyParts);

        QString childName = segments.first();
        if (segments.length() == 1) {
            children_.push_back(new TreeNode(std::move(childName), TreeNodeType::File, this));
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

        auto* child = new TreeNode(std::move(childName), TreeNodeType::Dir, this);
        children_.push_back(child);
        return child;
    }

    int TreeNode::row() const {
        return parent_
               ? static_cast<int>(parent_->children_.indexOf(this))
               : 1;
    }

    int TreeNode::childCount() const {
        return children_.length();
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

    RootNode::RootNode(QString fileName)
            : TreeNode("", TreeNodeType::Root, nullptr) {
        children_.append(new TreeNode(std::move(fileName), this));
    }

    void RootNode::addEntry(const PboEntry* entry) {
        children_.first()->addEntry(entry);
    }
}