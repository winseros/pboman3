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

    void PboNode::addEntry(const PboPath& entryPath) {
        QPointer<PboNode> parent = createFolderHierarchy(entryPath);
        createFileNode(entryPath.last(), parent);
    }

    QPointer<PboNode> PboNode::addEntry(const PboPath& entryPath, const OnConflict& onConflict) {
        QPointer<PboNode> result;
        const QPointer<PboNode> existing = get(entryPath);
        if (existing) {
            if (existing->nodeType_ != PboNodeType::File) {
                throw PboTreeException("Can add only file entries here");
            }
            const PboConflictResolution resolution = onConflict(existing->makePath(), existing->nodeType_);
            switch (resolution) {
                case PboConflictResolution::Replace: {
                    const PboPath existingPath = existing->makePath();

                    QPointer<PboNode> parent = existing->par_;
                    assert(parent->children_.removeOne(existing) && "Must have deleted the node");
                    delete existing.get();//TODO: Check!

                    const PboNodeRemovedEvent evtR(&existingPath);
                    root_->onEvent(&evtR);

                    result = createFileNode(entryPath.last(), parent);
                    break;
                }
                case PboConflictResolution::Copy: {
                    QPointer<PboNode> parent = existing->par_;
                    const QString title = resolveNameConflict(parent, existing);
                    result = createFileNode(title, parent);
                    break;
                }
                case PboConflictResolution::Abort: {
                    break;
                }
            }
        } else {
            QPointer<PboNode> parent = createFolderHierarchy(entryPath);
            result = createFileNode(entryPath.last(), parent);
        }

        return result;
    }

    QPointer<PboNode> PboNode::get(const PboPath& node) const {
        if (node.isEmpty())
            return QPointer<PboNode>(const_cast<PboNode*>(this));

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

    QList<QPointer<PboNode>>::const_iterator PboNode::end() const {
        return children_.end();
    }

    QList<QPointer<PboNode>>::iterator PboNode::begin() {
        return children_.begin();
    }

    QList<QPointer<PboNode>>::iterator PboNode::end() {
        return children_.end();
    }

    QList<QPointer<PboNode>>::const_iterator PboNode::begin() const {
        return children_.begin();
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

    void PboNode::renameNode(const PboPath& node, const QString& title) const {
        QPointer<PboNode> n = get(node);
        assert(n && "The node must exist");

        const QPointer<PboNode> existing = n->par_->findChild(title);
        if (!existing) {
            n->title_ = title;

            const PboNodeRenamedEvent evt(&node, title);
            root_->onEvent(&evt);
        }
    }

    void PboNode::removeNode(const PboPath& node) const {
        QPointer<PboNode> n = get(node);
        if (!n)
            throw PboTreeException("The node must exist");

        PboPath nodePath;

        do {
            nodePath = n->makePath();

            QPointer<PboNode> p = n->par_;
            p->children_.removeOne(n);
            delete n.get();
            n = p;
        } while (n->nodeType_ != PboNodeType::Container && n->childCount() == 0);

        const PboNodeRemovedEvent evt(&nodePath);
        root_->onEvent(&evt);
    }

    QPointer<PboNode> PboNode::createFolderHierarchy(const PboPath& path) {
        QPointer<PboNode> parent = this;
        for (int i = 0; i < path.length() - 1; i++) {
            QPointer<PboNode> child = parent->findChild(path.at(i));
            if (!child) {
                child = QPointer<PboNode>(new PboNode(path.at(i), PboNodeType::Folder, parent, root_));
                parent->children_.append(child);

                const PboPath childPath = child->makePath();
                const PboNodeCreatedEvent evt(&childPath, child->nodeType_);
                root_->onEvent(&evt);
            }
            parent = child;
        }
        return parent;
    }

    QPointer<PboNode> PboNode::createFileNode(const QString& title, QPointer<PboNode>& parent) const {
        QPointer<PboNode> node(new PboNode(title, PboNodeType::File, parent, root_));
        parent->children_.append(node);

        const auto nodePath = node->makePath();
        const PboNodeCreatedEvent evt(&nodePath, node->nodeType_);
        root_->onEvent(&evt);
        return node;
    }

    QPointer<PboNode> PboNode::findChild(const QString& title) const {
        for (const QPointer<PboNode>& child : children_) {
            if (child->title_ == title)
                return child;
        }
        return nullptr;
    }

    QString PboNode::resolveNameConflict(const QPointer<PboNode>& parent, const QPointer<PboNode>& node) const {
        assert(node->nodeType_ == PboNodeType::File && "The conflicts must occur only between files");

        const qsizetype extensionPoint = node->title_.lastIndexOf(".");
        const QString extName = extensionPoint >= 0 ? node->title_.right(node->title_.length() - extensionPoint) : "";
        const QString fileName = node->title_.left(extensionPoint);

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
}
