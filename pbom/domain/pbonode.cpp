#include "pbonode.h"
#include <QRegularExpression>
#include "exception.h"
#include "validationexception.h"
#include "pbonodetransaction.h"
#include "func.h"
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

        const qsizetype index = p->children_.indexOf(node);
        node->parentNode_->children_.remove(index, 1);

        emit p->childRemoved(index);
        p->emitHierarchyChanged();
    }

    const QString& PboNode::title() const {
        return title_;
    }

    PboNodeType PboNode::nodeType() const {
        return nodeType_;
    }

    PboNode* PboNode::get(const PboPath& path) {
        PboNode* result = this;
        auto it = path.begin();
        while (it != path.end()) {
            result = FindDirectChild(result, *it);
            if (!result)
                return nullptr;
            ++it;
        }
        return result;
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
        if (nodeType_ == node.nodeType_) {
            if (nodeType_ == PboNodeType::Folder) {
                return title_ < node.title_; //folders alphabetically
            }
            QString name1, ext1;
            FileNames::splitByNameAndExtension(title_, name1, ext1);
            QString name2, ext2;
            FileNames::splitByNameAndExtension(node.title_, name2, ext2);
            if (name1 == name2) {
                return ext1 < ext2; //files alphabetically by extension
            }
            return name1 < name2; //files alphabetically by name
        }

        return nodeType_ > node.nodeType_; //folders first
    }

    PboNode* PboNode::createHierarchy(const PboPath& entryPath, const ConflictResolution& onConflict,
                                      bool emitEvents) {
        PboNode* node = this;
        for (qsizetype i = 0; i < entryPath.length() - 1; i++) {
            PboNode* folder = FindDirectChild(node, entryPath.at(i));
            if (!folder) {
                folder = node->createChild(entryPath.at(i), PboNodeType::Folder);
            } else if (folder->nodeType_ == PboNodeType::File) {
                switch (onConflict) {
                    case ConflictResolution::Replace: {
                        PboNode* replaceFolder = folder->parentNode_->createChild(
                            entryPath.last(), PboNodeType::File);
                        folder->removeFromHierarchy();
                        folder = replaceFolder;
                        break;
                    }
                    case ConflictResolution::Copy: {
                        const QString folderTitle = pickFolderTitle(node, entryPath.at(i));
                        folder = node->createChild(folderTitle, PboNodeType::Folder);
                        break;
                    }
                    default:
                        throw InvalidOperationException("Unsupported conflict resolution strategy");
                }
            }
            node = folder;
        }

        PboNode* file = FindDirectChild(node, entryPath.last());
        if (!file) {
            file = node->createChild(entryPath.last(), PboNodeType::File);
            if (emitEvents)
                emitHierarchyChanged();
        } else {
            switch (onConflict) {
                case ConflictResolution::Replace: {
                    PboNode* replaceFile = file->parentNode_->createChild(entryPath.last(), PboNodeType::File);
                    //this emits "hierarchyChanged"; this branch never called with emitEvents==false
                    file->removeFromHierarchy();
                    file = replaceFile;
                    break;
                }
                case ConflictResolution::Copy: {
                    const QString fileTitle = pickFileTitle(node, entryPath.last());
                    file = node->createChild(fileTitle, PboNodeType::File);
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

    QString PboNode::pickFolderTitle(const PboNode* parent, const QString& expectedTitle) const {
        int index = 1;
        QString attemptTitle = FileNames::getCopyFolderName(expectedTitle, index);
        auto it = parent->children_.constBegin();
        while (it != parent->children_.constEnd()) {
            if ((*it)->title() == attemptTitle && (*it)->nodeType_ == PboNodeType::File) {
                index++;
                attemptTitle = FileNames::getCopyFolderName(expectedTitle, index);
                it = parent->children_.constBegin();
            } else {
                ++it;
            }
        }
        return attemptTitle;
    }

    QString PboNode::pickFileTitle(const PboNode* parent, const QString& expectedTitle) const {
        int index = 1;
        QString attemptTitle = FileNames::getCopyFileName(expectedTitle, index);
        auto it = parent->children_.constBegin();
        while (it < parent->children_.constEnd()) {
            if ((*it)->title() == attemptTitle) {
                index++;
                attemptTitle = FileNames::getCopyFileName(expectedTitle, index);
                it = parent->children_.constBegin();
            } else {
                ++it;
            }
        }
        return attemptTitle;
    }

    PboNode* PboNode::createChild(const QString& title, PboNodeType nodeType) {
        const auto child = QSharedPointer<PboNode>(new PboNode(title, nodeType, this));
        const qsizetype index = getChildListIndex(child.get());
        children_.insert(index, child);
        emit childCreated(child.get(), index);
        return child.get();
    }

    qsizetype PboNode::getChildListIndex(const PboNode* node) const {
        qsizetype index = 0;
        for (const QSharedPointer<PboNode>& sibling : children_) {
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

    void PboNode::setTitle(QString title) {
        if (title != title_) {

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

    QDebug operator<<(QDebug debug, const PboNode& node) {
        return node.parentNode_
                   ? debug << "PboNode(Path=" << node.makePath() << ")"
                   : debug << "PboNode(RootTitle=" << node.title_ << ")";

    }
}
