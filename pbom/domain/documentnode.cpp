#include "documentnode.h"
#include <QRegularExpression>
#include "util/exception.h"
#include "validationexception.h"

namespace pboman3::domain {
    DocumentNode::DocumentNode(QString title, PboNodeType nodeType, DocumentNode* parentNode)
        : AbstractNode(parentNode),
          nodeType_(nodeType),
          title_(std::move(title)) {
        if (title_.isEmpty())
            throw ValidationException("Title must not be empty");
    }

    DocumentNode* DocumentNode::createHierarchy(const PboPath& entryPath) {
        return createHierarchy(entryPath, ConflictResolution::Copy, false);
    }

    DocumentNode* DocumentNode::createHierarchy(const PboPath& entryPath, const ConflictResolution& onConflict) {
        return createHierarchy(entryPath, onConflict, true);
    }

    void DocumentNode::removeFromHierarchy() {
        if (!parentNode_)
            throw InvalidOperationException("Must not be called on the root node");

        DocumentNode* node = this;
        while (node->parentNode_
            && node->parentNode_->nodeType_ != PboNodeType::Container
            && node->parentNode_->count() == 1) {
            node = node->parentNode_;
        }

        DocumentNode* p = node->parentNode_;
        assert(p && "Must not be null");

        const qsizetype index = p->children_.indexOf(node);
        node->parentNode_->children_.remove(index, 1);

        emit p->childRemoved(index);
        p->emitHierarchyChanged();
    }

    bool DocumentNode::isPathConflict(const PboPath& path) const {
        const DocumentNode* node = this;
        for (qsizetype i = 0; i < path.length() - 1; i++) {
            const DocumentNode* folder = node->findChild(path.at(i));
            if (folder) {
                if (folder->nodeType_ == PboNodeType::File)
                    return true;
                node = folder;
            } else {
                return false;
            }
        }

        const DocumentNode* file = node->findChild(path.last());
        return file;
    }

    const QString& DocumentNode::title() const {
        return title_;
    }

    void DocumentNode::setTitle(QString title) {
        if (title != title_) {
            // if (const TitleError err = verifyTitle(title); err != nullptr)
            //     throw InvalidOperationException(err);

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

    PboNodeType DocumentNode::nodeType() const {
        return nodeType_;
    }

    DocumentNode* DocumentNode::get(const PboPath& path) {
        DocumentNode* result = this;
        auto it = path.begin();
        while (it != path.end()) {
            result = result->findChild(*it);
            if (!result)
                return nullptr;
            ++it;
        }
        return result;
    }

    PboPath DocumentNode::makePath() const {
        PboPath path;
        path.reserve(depth());

        const DocumentNode* p = this;
        while (p->parentNode_) {
            path.prepend(p->title_);
            p = p->parentNode_;
        }
        return path;
    }

    bool DocumentNode::operator<(const DocumentNode& node) const {
        if (nodeType_ == node.nodeType_) {
            if (nodeType_ == PboNodeType::Folder) {
                return title_ < node.title_; //folders alphabetically
            }
            QString name1, ext1;
            SplitByNameAndExtension(title_, name1, ext1);
            QString name2, ext2;
            SplitByNameAndExtension(node.title_, name2, ext2);
            if (name1 == name2) {
                return ext1 < ext2; //files alphabetically by extension
            }
            return name1 < name2; //files alphabetically by name
        }

        return nodeType_ > node.nodeType_; //folders first
    }

    DocumentNode* DocumentNode::createHierarchy(const PboPath& entryPath, const ConflictResolution& onConflict,
                                      bool emitEvents) {
        DocumentNode* node = this;
        for (qsizetype i = 0; i < entryPath.length() - 1; i++) {
            DocumentNode* folder = node->findChild(entryPath.at(i));
            if (!folder) {
                folder = node->createChild(entryPath.at(i), PboNodeType::Folder);
            } else if (folder->nodeType_ == PboNodeType::File) {
                switch (onConflict) {
                    case ConflictResolution::Replace: {
                        DocumentNode* replaceFolder = folder->parentNode_->createChild(
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

        DocumentNode* file = node->findChild(entryPath.last());
        if (!file) {
            file = node->createChild(entryPath.last(), PboNodeType::File);
            if (emitEvents)
                emitHierarchyChanged();
        } else {
            switch (onConflict) {
                case ConflictResolution::Replace: {
                    DocumentNode* replaceFile = file->parentNode_->createChild(entryPath.last(), PboNodeType::File);
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

    DocumentNode* DocumentNode::findChild(const QString& title) const {
        for (const QSharedPointer<DocumentNode>& child : children_) {
            if (child->title_ == title)
                return child.get();
        }
        return nullptr;
    }

    QString DocumentNode::pickFolderTitle(const DocumentNode* parent, const QString& expectedTitle) const {
        int index = 1;
        QString attemptTitle = formatFolderTitleCopy(expectedTitle, index);
        auto it = parent->children_.constBegin();
        while (it != parent->children_.constEnd()) {
            if ((*it)->title() == attemptTitle && (*it)->nodeType_ == PboNodeType::File) {
                index++;
                attemptTitle = formatFolderTitleCopy(expectedTitle, index);
                it = parent->children_.constBegin();
            } else {
                ++it;
            }
        }
        return attemptTitle;
    }

    QString DocumentNode::pickFileTitle(const DocumentNode* parent, const QString& expectedTitle) const {
        int index = 1;
        QString attemptTitle = formatFileTitleCopy(expectedTitle, index);
        auto it = parent->children_.constBegin();
        while (it < parent->children_.constEnd()) {
            if ((*it)->title() == attemptTitle) {
                index++;
                attemptTitle = formatFileTitleCopy(expectedTitle, index);
                it = parent->children_.constBegin();
            } else {
                ++it;
            }
        }
        return attemptTitle;
    }

    QString DocumentNode::formatFolderTitleCopy(const QString& expectedTitle, qsizetype copyIndex) const {
        return expectedTitle + "(" + QString::number(copyIndex) + ")";
    }

    QString DocumentNode::formatFileTitleCopy(const QString& expectedTitle, qsizetype copyIndex) const {
        QString expectedName, expectedExt;
        SplitByNameAndExtension(expectedTitle, expectedName, expectedExt);
        return expectedName + "(" + QString::number(copyIndex) + ")." + expectedExt;
    }

    DocumentNode* DocumentNode::createChild(const QString& title, PboNodeType nodeType) {
        const auto child = QSharedPointer<DocumentNode>(new DocumentNode(title, nodeType, this));
        const qsizetype index = getChildListIndex(child.get());
        children_.insert(index, child);
        emit childCreated(child.get(), index);
        return child.get();
    }

    qsizetype DocumentNode::getChildListIndex(const DocumentNode* node) const {
        qsizetype index = 0;
        for (const QSharedPointer<DocumentNode>& sibling : children_) {
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

    void DocumentNode::emitHierarchyChanged() {
        DocumentNode* parent = this;
        while (parent->parentNode_) {
            parent = parent->parentNode_;
        }
        emit parent->hierarchyChanged();
    }

    QDebug operator<<(QDebug debug, const DocumentNode& node) {
        return debug << "DocumentNode(" << node.makePath() << ")";
    }
}
