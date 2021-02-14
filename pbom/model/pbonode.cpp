#include "pbonode.h"
#include <QRegularExpression>
#include "pbotreeexception.h"

namespace pboman3 {

    PboNode::PboNode(QString title, PboNodeType nodeType, const QPointer<PboNode>& par, const QPointer<PboNode>& root)
        : QObject(),
          nodeType_(nodeType),
          title_(std::move(title)),
          par_(par),
          root_(root ? root : QPointer<PboNode>(this)) {
    }

    PboNode::~PboNode() {
        qDeleteAll(children_);
    }

    void PboNode::addEntry(const PboEntry* entry) {
        const QRegularExpression reg("\\\\|/");
        const QList<QString> segs = entry->fileName.split(reg, Qt::SplitBehaviorFlags::SkipEmptyParts);

        PboNode* parent = this;
        for (int i = 0; i < segs.length() - 1; i++) {
            QPointer<PboNode> child = parent->findChild(segs.at(i));
            if (!child) {
                child = QPointer<PboNode>(new PboNode(segs.at(i), PboNodeType::Folder, parent, root_));
                parent->children_.append(child);

                const PboPath nodePath = child->makePath();
                const PboNodeCreatedEvent evt(&nodePath, child->nodeType_);
                root_->onEvent(&evt);
            }
            parent = child.get();
        }

        const QPointer<PboNode> node(new PboNode(segs.last(), PboNodeType::File, parent, root_));
        parent->children_.append(node);

        const PboPath nodePath = node->makePath();
        const PboNodeCreatedEvent evt(&nodePath, node->nodeType_);
        root_->onEvent(&evt);
    }

    QPointer<PboNode> PboNode::get(const PboPath& node) const {
        QPointer<PboNode> result = findChild(node.first());
        if (!result)
            return nullptr;

        auto it = node.begin();
        ++it;

        while (it != node.end()) {
            result = result->findChild(*it);
            if (!result)
                return nullptr;
            ++it;
        }
        return result;
    }


    QPointer<PboNode> PboNode::child(int index) const {
        return QPointer<PboNode>(children_.at(index).get());
    }

    int PboNode::childCount() const {
        return static_cast<int>(children_.count());
    }

    PboPath PboNode::makePath() const {
        PboPath path;
        const PboNode* p = this;
        while (p->par_) {
            path.prepend(p->title_);
            p = p->par_;
        }
        return path;
    }

    PboNodeType PboNode::nodeType() const {
        return nodeType_;
    }

    const QString& PboNode::title() const {
        return title_;
    }

    const QPointer<PboNode>& PboNode::par() const {
        return par_;
    }

    const QPointer<PboNode>& PboNode::root() const {
        return root_;
    }


    void PboNode::addNode(const PboPath& node) {

    }

    void PboNode::moveNode(const PboPath& node, const PboPath& newParent,
                           PboConflictResolution (*onConflict)(const PboPath&, PboNodeType)) {
        QPointer<PboNode> n = get(node);
        assert(n && "The node must exist");

        QPointer<PboNode> np = newParent.length() ? get(newParent) : QPointer<PboNode>(this);
        assert(np && "The new parent must exist");

        if (np->nodeType_ == PboNodeType::Folder || np->nodeType_ == PboNodeType::Container) {
            QPointer<PboNode> op = n->par_;
            moveNodeImpl(n, np, onConflict);
            cleanupNodeHierarchy(op);
        } else {
            throw PboTreeException("Only a Folder or Container node can be a parent");
        }
    }

    void PboNode::renameNode(const PboPath& node, const QString& title) {
        QPointer<PboNode> n = get(node);
        assert(n && "The node must exist");

        const QPointer<PboNode> existing = n->par_->findChild(title);
        if (!existing) {
            n->title_ = title;

            const PboNodeRenamedEvent evt(&node, title);
            root_->onEvent(&evt);
        }
    }

    void PboNode::removeNode(const PboPath& node) {
        QPointer<PboNode> n = get(node);
        assert(n && "The node must exist");
        QPointer<PboNode> op = n->par_;
        op->children_.removeOne(n);
        delete n.get();
        cleanupNodeHierarchy(op);

        const PboNodeRemovedEvent evt(&node);
        root_->onEvent(&evt);
    }

    QPointer<PboNode> PboNode::findChild(const QString& title) const {
        for (const QPointer<PboNode>& child : children_) {
            if (child->title_ == title)
                return child;
        }
        return nullptr;
    }

    QString PboNode::resolveNameConflict(const QPointer<PboNode>& parent, const QPointer<PboNode>& node) const {
        QString fileName;
        QString extName;
        if (node->nodeType_ == PboNodeType::File) {
            const qsizetype extensionPoint = node->title_.lastIndexOf(".");
            extName = extensionPoint >= 0 ? node->title_.right(node->title_.length() - extensionPoint) : "";
            fileName = node->title_.left(extensionPoint);
        } else {
            fileName = node->title();
        }

        int i = 1;
        QString newName = fileName;
        newName.append("-copy").append(extName);

        auto it = parent->children_.begin();
        while (it != parent->children_.end()) {
            if ((*it)->title_ == newName) {
                newName = fileName;
                newName.append("-copy").append(QString::number(i)).append(extName);
                it = parent->children_.begin();
                i++;
            } else {
                ++it;
            }
        }

        return newName;
    }

    void PboNode::moveNodeImpl(QPointer<PboNode>& node, QPointer<PboNode>& newParent,
                               PboConflictResolution (* onConflict)(const PboPath&, PboNodeType)) {

        QPointer<PboNode> oldParent = node->par_;

        auto existing = newParent->findChild(node->title_);
        if (existing && existing->nodeType_ == node->nodeType_) {
            const PboConflictResolution resolution = onConflict(node->makePath(), node->nodeType_);
            switch (resolution) {
                case PboConflictResolution::ReplaceOrMerge: {
                    if (node->nodeType_ == PboNodeType::File) {
                        const PboPath existingPath = existing->makePath();

                        newParent->children_.removeOne(existing);
                        delete existing.get();

                        const PboNodeRemovedEvent evtR(&existingPath);
                        root_->onEvent(&evtR);

                        const PboPath prevNodePath = node->makePath();
                        oldParent->children_.removeOne(node);
                        newParent->children_.append(node);
                        node->par_ = QPointer<PboNode>(newParent.get());

                        const PboPath newNodePath = node->makePath();
                        const PboNodeMovedEvent evtM(&prevNodePath, &newNodePath);
                        root_->onEvent(&evtM);
                    } else {
                        for (QPointer<PboNode>& child : node->children_) {
                            moveNodeImpl(child, existing, onConflict);
                        }
                        tryCleanupNode(node);
                    }
                    break;
                }
                case PboConflictResolution::Copy: {
                    const PboPath prevNodePath = node->makePath();

                    node->title_ = resolveNameConflict(newParent, node);
                    oldParent->children_.removeOne(node);
                    newParent->children_.append(node);
                    node->par_ = QPointer<PboNode>(newParent.get());

                    const PboPath newNodePath = node->makePath();
                    const PboNodeMovedEvent evt(&prevNodePath, &newNodePath);
                    root_->onEvent(&evt);

                    break;

                }
                case PboConflictResolution::Abort: {
                    return;
                }
            }
        } else {
            const PboPath prevNodePath = node->makePath();

            oldParent->children_.removeOne(node);
            newParent->children_.append(node);
            node->par_ = QPointer<PboNode>(newParent.get());

            const PboPath newNodePath = node->makePath();
            const PboNodeMovedEvent evt(&prevNodePath, &newNodePath);
            root_->onEvent(&evt);
        }
    }

    void PboNode::cleanupNodeHierarchy(QPointer<PboNode>& node) {
        while (node->nodeType_ != PboNodeType::Container && node->childCount() == 0) {
            const PboPath nodePath = node->makePath();

            QPointer<PboNode> p = node->par_;
            p->children_.removeOne(node);
            delete node.get();
            node = p;

            const PboNodeRemovedEvent evt(&nodePath);
            root_->onEvent(&evt);
        }
    }

    bool PboNode::tryCleanupNode(QPointer<PboNode>& node) {
        assert(node->nodeType_ != PboNodeType::File && "File node can not be cleaned up");
        if (node->nodeType_ == PboNodeType::Folder && node->childCount() == 0) {
            const PboPath nodePath = node->makePath();

            node->par_->children_.removeOne(node);
            delete node.get();

            const PboNodeRemovedEvent evt(&nodePath);
            root_->onEvent(&evt);

            return true;
        }
        return false;
    }
}
