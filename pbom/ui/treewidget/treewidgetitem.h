#pragma once

#include <QTreeWidgetItem>
#include "domain/pbonode.h"
#include "ui/iconmgr.h"

namespace pboman3 {
    using namespace domain;

    class TreeWidgetItem : public QTreeWidgetItem, public QObject {
    public:
        TreeWidgetItem(PboNode* node);

        TreeWidgetItem(PboNode* node, const QSharedPointer<IconMgr>& icons);

        PboNode* node() const;

        void rename() const;

        void folderSyncIcon();

    private:
        PboNode* node_;
        QSharedPointer<IconMgr> icons_;

    private slots:
        void initCreateChildren();

        void onNodeTitleChanged(const QString& title);

        void onNodeChildCreated(PboNode* node, qsizetype index);

        void onNodeChildMoved(qsizetype prevIndex, qsizetype newIndex);

        void onNodeChildRemoved(qsizetype index);
    };
}
