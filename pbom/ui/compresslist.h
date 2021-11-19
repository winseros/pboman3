#pragma once

#include <QTreeWidget>
#include "model/interactionparcel.h"

namespace pboman3::ui {
    using namespace model;

    class CompressListItem : public QTreeWidgetItem {
    public:
        CompressListItem(int id)
            : QTreeWidgetItem(),
              id_(id) {

        }

        int id() const { return id_; }

    private:
        int id_;
    };

    class CompressList : public QTreeWidget {
    public:
        CompressList(QWidget* parent = nullptr);

        void setDataSource(NodeDescriptors* descriptors);

    protected:
        void showEvent(QShowEvent* event) override;

        void keyReleaseEvent(QKeyEvent* event) override;

    private:
        NodeDescriptors* descriptors_;

        void toggleSelectedItems() const;

        void onItemChanged(QTreeWidgetItem* item, int column) const;
    };
}
