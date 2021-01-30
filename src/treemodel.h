#pragma once

#include <QAbstractItemModel>
#include <QRegularExpression>
#include <QString>
#include <QVector>
#include "model/pbomodel.h"

namespace pboman3 {
    enum TreeNodeType {
        File,
        Dir,
        Container
    };

    class TreeNode {
    public:
        TreeNode(QString title, TreeNodeType nodeType);
        ~TreeNode();

        void addEntry(const PboEntry* entry);

        void setParent(const TreeNode* parent);

        const TreeNode* parent() const;

        const QString& path() const;

        const QString& title() const;

        int row() const;

        int childCount() const;

        const TreeNode* child(int index);
    private:
        QVector<TreeNode*> children_;
        TreeNode* parent_;
        TreeNodeType nodeType_;
        QString title_;
        QString path_;

        TreeNode* getOrCreateChild(QString& childName);
    };

    class TreeModel : public QAbstractItemModel {
    public:
        TreeModel(const PboModel* model);

        QModelIndex index(int row, int column, const QModelIndex& parent) const override;

        QModelIndex parent(const QModelIndex& child) const override;

        int rowCount(const QModelIndex& parent) const override;

        int columnCount(const QModelIndex& parent) const override;

        QVariant data(const QModelIndex& index, int role) const override;

        Qt::ItemFlags flags(const QModelIndex& index) const override;

    private:
        QRegularExpression pathSep_;
        const PboModel* model_;
        unique_ptr<TreeNode> root_;
    private slots:

        void onModelEvent(const PboModelEvent* event);
    };
}
