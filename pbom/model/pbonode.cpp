#include "pbonode.h"
#include <QRegularExpression>
#include "pbotreeexception.h"

namespace pboman3 {
    PboNode::PboNode(QString title, PboNodeType nodeType, PboNode* par, PboNode* root)
        : QObject(),
          nodeType_(nodeType),
          title_(std::move(title)),
          par_(par),
          root_(root ? root : this) {
    }

    PboNode::~PboNode() {
        qDeleteAll(children_);
    }

    PboNode* PboNode::addEntry(const PboPath& entryPath) {
        PboNode* parent = createFolderHierarchy(entryPath);
        PboNode* node = createFileNode(entryPath.last(), parent);
        return node;
    }

    PboNode* PboNode::addEntry(const PboPath& entryPath, const ConflictResolution& onConflict) {
        PboNode* result = nullptr;
        if (const PboNode* existing = get(entryPath)) {
            switch (onConflict) {
                case ConflictResolution::Replace: {
                    const PboPath existingPath = existing->makePath();

                    PboNode* parent = existing->par_;
                    assert(parent->children_.removeOne(existing) && "Must have deleted the node");
                    delete existing;

                    const PboNodeRemovedEvent evtR(&existingPath);
                    root_->onEvent(&evtR);

                    result = createFileNode(entryPath.last(), parent);
                    break;
                }
                case ConflictResolution::Copy: {
                    PboNode* parent = existing->par_;
                    const QString title = resolveNameConflict(parent, existing);
                    result = createFileNode(title, parent);
                    break;
                }
                case ConflictResolution::Skip: {
                    throw PboTreeException("Unsupported conflict resolution strategy: Skip");
                }
                case ConflictResolution::Unset: {
                    throw PboTreeException("Unsupported conflict resolution strategy: Unset");
                }
            }
        } else {
            PboNode* parent = createFolderHierarchy(entryPath);
            result = createFileNode(entryPath.last(), parent);
        }

        return result;
    }

    bool PboNode::fileExists(const PboPath& path) {
        const PboNode* node = get(path);
        return node && node->nodeType_ == PboNodeType::File;
    }

    PboNode* PboNode::get(const PboPath& path) {
        if (path.isEmpty())
            return this;

        PboNode* result = findChild(path.first());
        if (!result)
            return nullptr;

        auto it = path.begin();
        ++it;

        while (it != path.end()) {
            result = result->findChild(*it);
            if (!result)
                return nullptr;
            ++it;
        }
        return result;
    }

    PboNode* PboNode::child(int index) const {
        return children_.at(index);
    }

    QList<PboNode*>::const_iterator PboNode::end() const {
        return children_.end();
    }

    QList<PboNode*>::iterator PboNode::begin() {
        return children_.begin();
    }

    QList<PboNode*>::iterator PboNode::end() {
        return children_.end();
    }

    QList<PboNode*>::const_iterator PboNode::begin() const {
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

    PboNode* PboNode::par() const {
        return par_;
    }

    PboNode* PboNode::root() const {
        return root_;
    }

    void PboNode::renameNode(const PboPath& node, const QString& title, const ConflictResolution& onConflict) {
        PboNode* n = get(node);
        assert(n && "The node must exist");

        const PboNode* existing = n->par_->findChild(title);
        if (existing) {
            switch (onConflict) {
                case ConflictResolution::Copy: {
                    n->title_ = resolveNameConflict(n->par_, existing);
                    const PboNodeRenamedEvent evt(&node, title);
                    root_->onEvent(&evt);
                    break;
                }
                case ConflictResolution::Replace: {
                    throw PboTreeException("Unsupported conflict resolution strategy: Replace");
                }
                case ConflictResolution::Skip: {
                    throw PboTreeException("Unsupported conflict resolution strategy: Skip");
                }
                case ConflictResolution::Unset: {
                    throw PboTreeException("Unsupported conflict resolution strategy: Unset");
                }
            }
        } else {
            n->title_ = title;
            const PboNodeRenamedEvent evt(&node, title);
            root_->onEvent(&evt);
        }
    }

    void PboNode::removeNode(const PboPath& node) {
        PboNode* n = get(node);
        if (!n)
            throw PboTreeException("The node must exist");

        PboPath nodePath;

        do {
            nodePath = n->makePath();

            PboNode* p = n->par_;
            p->children_.removeOne(n);
            delete n;
            n = p;
        } while (n->nodeType_ != PboNodeType::Container && n->childCount() == 0);

        const PboNodeRemovedEvent evt(&nodePath);
        root_->onEvent(&evt);
    }

    PboNode* PboNode::createFolderHierarchy(const PboPath& path) {
        PboNode* parent = this;
        for (int i = 0; i < path.length() - 1; i++) {
            PboNode* child = parent->findChild(path.at(i));
            if (!child) {
                child = new PboNode(path.at(i), PboNodeType::Folder, parent, root_);
                parent->children_.append(child);

                const PboPath childPath = child->makePath();
                const PboNodeCreatedEvent evt(&childPath, child->nodeType_);
                root_->onEvent(&evt);
            }
            parent = child;
        }
        return parent;
    }

    PboNode* PboNode::createFileNode(const QString& title, PboNode* parent) const {
        auto* node = new PboNode(title, PboNodeType::File, parent, root_);
        parent->children_.append(node);

        const auto nodePath = node->makePath();
        const PboNodeCreatedEvent evt(&nodePath, node->nodeType_);
        root_->onEvent(&evt);
        return node;
    }

    PboNode* PboNode::findChild(const QString& title) const {
        for (PboNode* child : children_) {
            if (child->title_ == title)
                return child;
        }
        return nullptr;
    }

    QString PboNode::resolveNameConflict(const PboNode* parent, const PboNode* node) const {
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
