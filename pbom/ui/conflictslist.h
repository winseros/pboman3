#pragma once

#include "QTreeWidget"
#include "model/conflictsparcel.h"

namespace pboman3::ui {
    using namespace model;

    class ConflictsListItem : public QTreeWidgetItem {
    public:
        ConflictsListItem(int id):
            QTreeWidgetItem(),
            id_(id) {

        }

        int id() const { return id_; }

    private:
        int id_;
    };

    class ConflictsList : public QTreeWidget {
    public:
        ConflictsList(QWidget* parent = nullptr);

        void setDataSource(NodeDescriptors* descriptors, ConflictsParcel* conflicts);

    protected:
        void showEvent(QShowEvent* event) override;

    private:
        NodeDescriptors* descriptors_;
        ConflictsParcel* conflicts_;
        QSharedPointer<QAction> actionSkip_;
        QSharedPointer<QAction> actionCopy_;
        QSharedPointer<QAction> actionReplace_;

        void setUpUiActions();

        void onContextMenuRequested(const QPoint& point) const;

        void onSkipClick() const;

        void onCopyClick() const;

        void onReplaceClick() const;

        void setConflictResolution(ConflictResolution resolution) const;

        static QString resolutionToText(ConflictResolution resolution);
    };
}
