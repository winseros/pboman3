#pragma once

#include <QFont>
#include <QIcon>
#include <QModelIndex>
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include "model/pbomodel.h"

namespace pboman3 {
    enum class TreeNodeType {
        Root,
        Container,
        Dir,
        File
    };

    class TreeNode : public QObject {
    Q_OBJECT

    public:
        TreeNode(QString title, TreeNodeType nodeType, const QPointer<TreeNode>& parentNode, const PboEntry* entry);

        TreeNode(QString containerName, const QPointer<TreeNode>& parentNode);

        friend bool operator<(const TreeNode& one, const TreeNode& two);

        virtual void addEntry(const PboEntry* entry);

        void scheduleRemove(const PboEntry* entry, bool schedule = true);

        QPointer<TreeNode> prepareRemove(const PboEntry* entry) const;

        void completeRemove(const PboEntry* entry);

        void collectEntries(QMap<const QString, const PboEntry*>& collection) const;

        QPointer<TreeNode> parentNode() const;

        void setParentNode(const QPointer<TreeNode>& parentNode);

        const PboEntry* entry() const;

        const QString& title() const;

        const QString& path() const;

        void expand(bool expand);

        QIcon icon() const;

        QFont font() const;

        int row() const;

        int childNodeCount() const;

        QPointer<TreeNode> childNode(int index) const;

    protected:
        QVector<QSharedPointer<TreeNode>> childNodes_;

    private:
        const PboEntry* entry_;
        QString title_;
        TreeNodeType nodeType_;
        QString path_;
        QPointer<TreeNode> parentNode_;
        bool expanded_;
        bool isPendingToBeRemoved_;

        QSharedPointer<TreeNode> getOrCreateChild(const QString& childName);

        QPointer<TreeNode> findFileNode(const PboEntry* entry) const;

        QList<QString> getPathComponents(const PboEntry* entry) const;

        void insertSorted(const QSharedPointer<TreeNode>& node);
    };

    class RootNode : public virtual TreeNode {
    public:
        RootNode(QString fileName);

        void addEntry(const PboEntry* pboEntry) override;
    };
}
