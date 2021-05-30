#pragma once

#include <QTreeWidgetItem>
#include "model/pbonode.h"

namespace pboman3 {
    class TreeWidgetItem : public QTreeWidgetItem, public QObject {
    public:
        TreeWidgetItem(PboNode* node);

        PboNode* node() const;

        void rename() const;

    private:
        PboNode* node_;

    private slots:
        void initCreateChildren();

        void onNodeTitleChanged(const QString& title);

        void onNodeChildCreated(PboNode* node, qsizetype index);

        void onNodeChildMoved(qsizetype prevIndex, qsizetype newIndex);

        void onNodeChildRemoved(qsizetype index);
    };
}
