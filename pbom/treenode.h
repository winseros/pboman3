#pragma once

#include <QIcon>
#include <QString>
#include "model/pbomodel.h"

namespace pboman3 {
    enum class TreeNodeType {
        Root,
        Container,
        Dir,
        File
    };

    class TreeNode {
    public:
        TreeNode(QString title, TreeNodeType nodeType, const TreeNode* parent, const PboEntry* entry);

        TreeNode(QString containerName, const TreeNode* parent);

        virtual ~TreeNode();

        virtual void addEntry(const PboEntry* pboEntry);

        void scheduleRemove(const PboEntry* pboEntry, bool schedule = true);

        void setParent(const TreeNode* parent);

        const TreeNode* parent() const;

        const QString& path() const;

        const QString& title() const;

        int row() const;

        int childCount() const;

        const TreeNode* child(int index) const;

        QIcon icon() const;

        void expand(bool expand);

        bool isPendingToBeRemoved() const;

        friend bool operator<(const TreeNode& one, const TreeNode& two);

        const PboEntry* entry;

        void collectEntries(QMap<const QString, const PboEntry*>& collection) const;

    protected:
        QVector<TreeNode*> children_;

    private:
        QString title_;
        TreeNodeType nodeType_;
        QString path_;
        const TreeNode* parent_;
        bool expanded_;
        bool isPendingToBeRemoved_;

        QList<QString> getChildPathSegments(const PboEntry* pboEntry) const;

        TreeNode* getOrCreateChild(const QString& childName);

        void insertSorted(TreeNode* node);
    };

    class RootNode : public virtual TreeNode {
    public:
        RootNode(QString fileName);

        void addEntry(const PboEntry* pboEntry) override;


    };
}
