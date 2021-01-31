#pragma once

#include <QString>
#include <QIcon>
#include "model/pbomodel.h"

namespace pboman3 {
    enum TreeNodeType {
        File,
        Dir,
        Container,
        Root
    };

    class TreeNode {
    public:
        TreeNode(QString title, TreeNodeType nodeType, const TreeNode* parent);

        TreeNode(QString containerName, const TreeNode* parent);

        virtual ~TreeNode();

        virtual void addEntry(const PboEntry* entry);

        void setParent(const TreeNode* parent);

        const TreeNode* parent() const;

        const QString& path() const;

        const QString& title() const;

        int row() const;

        int childCount() const;

        const TreeNode* child(int index);

        QIcon icon() const;

        void expand(bool expand);

    protected:
        QVector<TreeNode*> children_;

    private:
        const TreeNode* parent_;
        TreeNodeType nodeType_;
        QString title_;
        QString path_;
        bool expanded_;

        TreeNode* getOrCreateChild(QString& childName);
    };

    class RootNode : public virtual TreeNode {
    public:
        RootNode(QString fileName);

        void addEntry(const PboEntry* entry) override;
    };
}
