#include "pbonode.h"
#include <QRegularExpression>
#include "exception.h"
#include "validationexception.h"
#include "pbonodetransaction.h"
#include "util/filenames.h"

namespace pboman3::domain {
    PboNode::PboNode(QString title, PboNodeType nodeType, PboNode* parentNode)
        : AbstractNode(parentNode),
          nodeType_(nodeType),
          title_(std::move(title)) {
        if (title_.isEmpty())
            throw ValidationException("Title must not be empty");
    }

    PboNode* PboNode::createHierarchy(const PboPath& entryPath) {
        return createHierarchy(entryPath, ConflictResolution::Copy, false);
    }

    PboNode* PboNode::createHierarchy(const PboPath& entryPath, const ConflictResolution& onConflict) {
        return createHierarchy(entryPath, onConflict, true);
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
        p->removeNode(node->sharedFromThis());
        p->emitHierarchyChanged();
    }

    const QString& PboNode::title() const {
        return title_;
    }

    PboNodeType PboNode::nodeType() const {
        return nodeType_;
    }

    PboNode* PboNode::get(const PboPath& path) {
        QSharedPointer<PboNode> node = sharedFromThis();
        assert(node && "sharedFromThis() returned null");
        for (const QString& seg : path) {
            node = node->getChild(seg);
            if (!node)
                return nullptr;
        }
        return node.get();
    }

    const PboNode* PboNode::get(const PboPath& path) const {
        QSharedPointer<const PboNode> node = sharedFromThis();
        assert(node && "sharedFromThis() returned null");
        for (const QString& seg : path) {
            node = node->getChild(seg);
            if (!node)
                return nullptr;
        }
        return node.get();
    }

    PboPath PboNode::makePath() const {
        PboPath path;
        path.reserve(depth());

        const PboNode* p = this;
        while (p->parentNode_) {
            path.prepend(p->title_);
            p = p->parentNode_;
        }
        return path;
    }

    QSharedPointer<PboNodeTransaction> PboNode::beginTransaction() {
        return QSharedPointer<PboNodeTransaction>(new PboNodeTransaction(this));
    }

    bool PboNode::operator<(const PboNode& node) const {
        return nodeType_ == node.nodeType_
        ? title_ < node.title_
        : nodeType_ > node.nodeType_;
    }

    PboNode* PboNode::createHierarchy(const PboPath& entryPath, const ConflictResolution& onConflict,
                                      bool emitEvents) {
        PboNode* node = this;
        const auto last = --entryPath.end();
        auto it = entryPath.begin();
        while (it != last) {
            PboNode* folder = node->getChild(*it).get();
            if (!folder) {
                folder = node->createNode(*it, PboNodeType::Folder);
            } else if (folder->nodeType_ == PboNodeType::File) {
                switch (onConflict) {
                    case ConflictResolution::Replace: {
                        const auto p = folder->parentNode_;
                        p->removeNode(folder->sharedFromThis());
                        folder = p->createNode(*it, PboNodeType::Folder);
                        break;
                    }
                    case ConflictResolution::Copy: {
                        const QString folderTitle = node->pickFolderTitle(*it);
                        folder = node->createNode(folderTitle, PboNodeType::Folder);
                        break;
                    }
                    default:
                        throw InvalidOperationException("Unsupported conflict resolution strategy");
                }
            }
            node = folder;
            ++it;
        }

        PboNode* file = node->getChild(*last).get();
        if (!file) {
            file = node->createNode(*last, PboNodeType::File);
            if (emitEvents)
                emitHierarchyChanged();
        } else {
            switch (onConflict) {
                case ConflictResolution::Replace: {
                    node->removeNode(file->sharedFromThis());
                    file = node->createNode(*last, PboNodeType::File);
                    if (emitEvents)
                        emitHierarchyChanged();
                    break;
                }
                case ConflictResolution::Copy: {
                    const QString fileTitle = node->pickFileTitle(*last);
                    file = node->createNode(fileTitle, PboNodeType::File);
                    if (emitEvents)
                        emitHierarchyChanged();
                    break;
                }
                default:
                    throw InvalidOperationException("Unsupported conflict resolution strategy");
            }
        }

        return file;
    }

    QString PboNode::pickFolderTitle(const QString& expectedTitle) const {
        int index = 1;
        QString attemptTitle = FileNames::getCopyFolderName(expectedTitle, index);
        while (hasChild(attemptTitle)) {
            index++;
            attemptTitle = FileNames::getCopyFolderName(expectedTitle, index);
        }
        return attemptTitle;
    }

    QString PboNode::pickFileTitle(const QString& expectedTitle) const {
        int index = 1;
        QString attemptTitle = FileNames::getCopyFileName(expectedTitle, index);
        while (hasChild(attemptTitle)) {
            index++;
            attemptTitle = FileNames::getCopyFileName(expectedTitle, index);
        }
        return attemptTitle;
    }

    PboNode* PboNode::createNode(const QString& title, PboNodeType nodeType) {
        const QSharedPointer<PboNode> child(new PboNode(title, nodeType, this));
        const qsizetype insertedIndex = addChild(child);
        emit childCreated(child.get(), insertedIndex);
        return child.get();
    }

    void PboNode::removeNode(const QSharedPointer<PboNode>& node) {
        const qsizetype index = removeChild(node);
        emit childRemoved(index);
    }

    void PboNode::emitHierarchyChanged() {
        PboNode* parent = this;
        while (parent->parentNode_) {
            parent = parent->parentNode_;
        }
        emit parent->hierarchyChanged();
    }

    void PboNode::setTitle(QString title) {
        if (title != title_) {
            qsizetype prevIndex = 0;
            QSharedPointer<PboNode> ptr;

            if (parentNode_) {
                ptr = sharedFromThis();
                assert(ptr && "sharedFromThis() returned null");
                prevIndex = parentNode_->removeChild(ptr);
            }
            title_ = std::move(title);
            if (parentNode_) {
                const qsizetype newIndex = parentNode_->addChild(ptr);
                if (prevIndex != newIndex) {
                    emit parentNode_->childMoved(prevIndex, newIndex);
                }
            }

            emit titleChanged(title_);
            emitHierarchyChanged();
        }
    }

    QDebug operator<<(QDebug debug, const PboNode& node) {
        return node.parentNode_
                   ? debug << "PboNode(Path=" << node.makePath() << ")"
                   : debug << "PboNode(RootTitle=" << node.title_ << ")";

    }
}
