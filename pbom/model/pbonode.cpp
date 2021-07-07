#include "pbonode.h"
#include <QRegularExpression>
#include "pbonodeevents.h"
#include "util/exception.h"

namespace pboman3 {
    PboNode::PboNode(QString title, PboNodeType nodeType, PboNode* parentNode)
        : QObject(),
          nodeType_(nodeType),
          title_(std::move(title)),
          parentNode_(parentNode) {
    }

    PboNode* PboNode::createHierarchy(const PboPath& entryPath) {
        PboNode* parent = createHierarchyFolders(entryPath);
        PboNode* node = parent->createChild(entryPath.last(), PboNodeType::File);
        return node;
    }

    PboNode* PboNode::createHierarchy(const PboPath& entryPath, const ConflictResolution& onConflict) {
        PboNode* result = nullptr;
        if (PboNode* existing = get(entryPath)) {
            switch (onConflict) {
                case ConflictResolution::Replace: {
                    result = existing->parentNode_->createChild(entryPath.last(), PboNodeType::File);
                    existing->removeFromHierarchy();
                    break;
                }
                case ConflictResolution::Copy: {
                    const QString title = resolveNameConflict(existing->parentNode_, existing);
                    result = existing->parentNode_->createChild(title, PboNodeType::File);
                    emitHierarchyChanged();
                    break;
                }
                case ConflictResolution::Skip: {
                    throw InvalidOperationException("Unsupported conflict resolution strategy: Skip");
                }
                case ConflictResolution::Unset: {
                    throw InvalidOperationException("Unsupported conflict resolution strategy: Unset");
                }
            }
        } else {
            PboNode* parent = createHierarchyFolders(entryPath);
            result = parent->createChild(entryPath.last(), PboNodeType::File);
            emitHierarchyChanged();
        }

        return result;
    }

    void PboNode::removeFromHierarchy() {
        if (!parentNode_)
            throw InvalidOperationException("Must not be called on the root node");

        PboNode* node = this;
        while (node->parentNode_
            && node->parentNode_->nodeType_ != PboNodeType::Container
            && node->parentNode_->count() == 1) {
            node = node->parentNode_;
        }

        PboNode* p = node->parentNode_;
        assert(p && "Must not be null");

        const qsizetype index = p->children_.indexOf(node);
        node->parentNode_->children_.remove(index, 1);

        emit p->childRemoved(index);
        p->emitHierarchyChanged();
    }

    const QString& PboNode::title() const {
        return title_;
    }

    void PboNode::setTitle(QString title) {
        if (title != title_) {
            if (const TitleError err = verifyTitle(title); err != nullptr)
                throw InvalidOperationException(err);

            title_ = std::move(title);
            emit titleChanged(title_);

            if (parentNode_) {
                const qsizetype prevIndex = parentNode_->children_.indexOf(this);
                const qsizetype newIndex = parentNode_->getChildListIndex(this);
                if (prevIndex != newIndex) {
                    parentNode_->children_.move(prevIndex, newIndex);
                    emit parentNode_->childMoved(prevIndex, newIndex);
                }

            }
            emitHierarchyChanged();
        }
    }

    TitleError PboNode::verifyTitle(const QString& title) const {
        if (title == nullptr || title.isEmpty())
            return "The value can not be empty";
        if (!parentNode_)
            return "";
        PboNode* existing = parentNode_->findChild(title);
        return existing && existing != this ? "The item with this name already exists" : "";
    }

    PboNodeType PboNode::nodeType() const {
        return nodeType_;
    }

    PboNode* PboNode::parentNode() const {
        return parentNode_;
    }

    PboNode* PboNode::get(const PboPath& path) {
        PboNode* result = this;
        auto it = path.begin();
        while (it != path.end()) {
            result = result->findChild(*it);
            if (!result)
                return nullptr;
            ++it;
        }
        return result;
    }

    PboNode* PboNode::at(qsizetype index) const {
        return children_.at(index).get();
    }

    int PboNode::depth() const {
        int result = 0;
        PboNode* parentNode = parentNode_;
        while (parentNode) {
            result++;
            parentNode = parentNode->parentNode_;
        }
        return result;
    }

    int PboNode::count() const {
        return static_cast<int>(children_.count());
    }

    PboPath PboNode::makePath() const {
        PboPath path;
        const PboNode* p = this;
        while (p->parentNode_) {
            path.prepend(p->title_);
            p = p->parentNode_;
        }
        return path;
    }

    QPointerListIterator<PboNode> PboNode::begin() {
        return QPointerListIterator(children_.data());
    }

    QPointerListIterator<PboNode> PboNode::end() {
        return QPointerListIterator(children_.data() + children_.count());
    }

    QPointerListConstIterator<PboNode> PboNode::begin() const {
        return QPointerListConstIterator(children_.data());
    }

    QPointerListConstIterator<PboNode> PboNode::end() const {
        return QPointerListConstIterator(children_.data() + count());
    }

    bool PboNode::operator<(const PboNode& node) const {
        return nodeType_ == node.nodeType_
                   ? title_ < node.title_ //alphabetically
                   : nodeType_ > node.nodeType_; //folders first
    }

    PboNode* PboNode::createHierarchyFolders(const PboPath& path) {
        PboNode* parent = this;
        for (int i = 0; i < path.length() - 1; i++) {
            PboNode* child = parent->findChild(path.at(i), PboNodeType::Folder);
            if (!child)
                child = parent->createChild(path.at(i), PboNodeType::Folder);
            parent = child;
        }
        return parent;
    }

    PboNode* PboNode::findChild(const QString& title, PboNodeType nodeType) const {
        for (QSharedPointer<PboNode> child : children_) {
            if (child->title_ == title && child->nodeType_ == nodeType)
                return child.get();
        }
        return nullptr;
    }

    PboNode* PboNode::findChild(const QString& title) const {
        for (QSharedPointer<PboNode> child : children_) {
            if (child->title_ == title)
                return child.get();
        }
        return nullptr;
    }

    PboNode* PboNode::createChild(const QString& title, PboNodeType nodeType) {
        const auto child = QSharedPointer<PboNode>(new PboNode(title, nodeType, this));
        const qsizetype index = getChildListIndex(child.get());
        children_.insert(index, child);
        emit childCreated(child.get(), index);
        return child.get();
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

    qsizetype PboNode::getChildListIndex(const PboNode* node) const {
        qsizetype index = 0;
        for (QSharedPointer<PboNode> sibling : children_) {
            if (sibling != node) {
                if (*sibling < *node) {
                    index++;
                } else {
                    break;
                }
            }
        }
        return index;
    }

    void PboNode::emitHierarchyChanged() {
        PboNode* parent = this;
        while (parent->parentNode_) {
            parent = parent->parentNode_;
        }
        emit parent->hierarchyChanged();
    }

    QDebug operator<<(QDebug debug, const PboNode& node) {
        return debug << "PboNode(" << node.makePath() << ")";
    }
}
