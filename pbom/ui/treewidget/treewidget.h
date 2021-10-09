#pragma once

#include "deleteop.h"
#include "treewidgetbase.h"
#include "model/pbomodel.h"
#include <QFutureWatcher>

namespace pboman3 {
    class TreeWidget : public TreeWidgetBase {
    Q_OBJECT
    public:
        struct ActionState {
            bool canOpen;
            bool canCopy;
            bool canCut;
            bool canPaste;
            bool canRemove;
            bool canRename;
            bool canExtract;

            friend QDebug operator<<(QDebug debug, const ActionState& state) {
                return debug << "TreeWidget.ActionState(CanOpen=" << state.canOpen
                    << ", CanCopy=" << state.canCopy << ", CanCut=" << state.canCut
                    << ", CanPaste=" << state.canPaste << ", CanRemove=" << state.canRemove
                    << ", CanRename=" << state.canRename << ", CanExtract=" << state.canExtract << ")";
            }
        };

        explicit TreeWidget(QWidget* parent = nullptr);

        void selectionOpen();

        void selectionExtract(const QString& dir, const PboNode* relativeTo);

        void selectionCopy();

        void selectionCut();

        void selectionPaste();

        void selectionRemove() const;

        void selectionRename() const;

        void setWidgetModel(PboModel* model);

    signals:
        void backgroundOpStarted(QFuture<void> future);

        void backgroundOpStopped();

        void actionStateChanged(const ActionState& state);

    protected:
        void dragStarted(const QList<PboNode*>& items) override;

        void dragDropped(PboNode* target, const QMimeData* mimeData) override;

    private:
        PboModel* model_;
        DeleteOp delete_;
        QFutureWatcher<InteractionParcel> dragDropWatcher_;
        QFutureWatcher<InteractionParcel> cutCopyWatcher_;
        QFutureWatcher<QString> openWatcher_;
        QFutureWatcher<int> extractWatcher_;
        ActionState actionState_;

        void onDoubleClicked();

        void onSelectionChanged();

        QList<PboNode*> selectionCopyImpl();

        void openExecute();

        void extractExecute();

        void dragStartExecute();

        void copyOrCutExecute();

        void addFilesFromPbo(PboNode* target, const QMimeData* mimeData);

        void addFilesFromFilesystem(const QList<QUrl>& urls);
    };
}
