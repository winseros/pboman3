#include "treewidget.h"
#include <QClipboard>
#include <QMimeData>
#include <QtConcurrent/QtConcurrentRun>
#include <QDrag>
#include "ui/fscollector.h"
#include "ui/insertdialog.h"
#include "util/appexception.h"

namespace pboman3 {
#define MIME_TYPE_PBOMAN "application/pboman3"

    TreeWidget::TreeWidget(QWidget* parent)
        : TreeWidgetBase(parent),
          model_(nullptr),
          actionState_() {

        connect(&dragDropWatcher_, &QFutureWatcher<InteractionParcel>::finished, this, &TreeWidget::dragStartExecute);
        connect(&cutCopyWatcher_, &QFutureWatcher<InteractionParcel>::finished, this, &TreeWidget::copyOrCutExecute);
        connect(this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::onSelectionChanged);
    }

    void TreeWidget::selectionCopy() {
        if (!actionState_.canCopy)
            throw AppException("Copy action is not available");

        selectionCopyImpl();
    }

    void TreeWidget::selectionCut() {
        if (!actionState_.canCut)
            throw AppException("Cut action is not available");

        QList<PboNode*> nodes = selectionCopyImpl();
        delete_.schedule(std::move(nodes));
    }

    void TreeWidget::selectionPaste() {
        if (!actionState_.canPaste)
            throw AppException("Paste action is not available");

        if (PboNode* item = getCurrentFolder()) {
            QClipboard* clipboard = QGuiApplication::clipboard();
            const QMimeData* mimeData = clipboard->mimeData();
            if (mimeData->hasFormat(MIME_TYPE_PBOMAN)) {
                addFilesFromPbo(item, mimeData);
            } else if (mimeData->hasUrls()) {
                addFilesFromFilesystem(mimeData->urls());
            }
        }
    }

    void TreeWidget::selectionRemove() const {
        if (!actionState_.canRemove)
            throw AppException("Remove action is not available");

        const QList<PboNode*> selection = getSelectedHierarchies();
        for (PboNode* item : selection) {
            item->removeFromHierarchy();
        }
    }

    void TreeWidget::selectionRename() const {
        if (!actionState_.canRename)
            throw AppException("Rename action is not available");

        const auto* selected = dynamic_cast<TreeWidgetItem*>(currentItem());
        selected->rename();
    }

    void TreeWidget::setModel(PboModel2* model) {
        model_ = model;
    }

    void TreeWidget::dragStarted(const QList<PboNode*>& items) {
        emit backgroundOpStarted();

        const QFuture<InteractionParcel> future = QtConcurrent::run(
            [this](QPromise<InteractionParcel>& promise, const QList<PboNode*>& selection) {
                InteractionParcel data = model_->interactionPrepare(
                    selection, [&promise]() { return promise.isCanceled(); });
                promise.addResult(data);
            }, items);

        delete_.schedule(items);
        dragDropWatcher_.setFuture(future);
    }

    void TreeWidget::dragDropped(PboNode* target, const QMimeData* mimeData) {
        if (mimeData->hasFormat(MIME_TYPE_PBOMAN)) {
            addFilesFromPbo(target, mimeData);
        } else if (mimeData->hasUrls()) {
            addFilesFromFilesystem(mimeData->urls());
        }
    }

    void TreeWidget::onSelectionChanged() {
        actionState_.canOpen = getCurrentFile();
        actionState_.canRename = !!currentItem();

        auto canCopy = [this]() {
            QList<PboNode*> nodes = getSelectedHierarchies();
            if (!nodes.count())
                return false;
            const bool rootSelected = !std::any_of(nodes.begin(), nodes.end(), [](PboNode* n) {
                return !n->parentNode();
                });
            return rootSelected;
        };

        actionState_.canCopy = canCopy();
        actionState_.canCut = actionState_.canCopy;
        actionState_.canRemove = actionState_.canCopy;

        actionState_.canPaste = getCurrentFolder();

        emit actionStateChanged(actionState_);
    }

    QList<PboNode*> TreeWidget::selectionCopyImpl() {
        emit backgroundOpStarted();

        QList<PboNode*> nodes = getSelectedHierarchies();

        const QFuture<InteractionParcel> future = QtConcurrent::run(
            [this](QPromise<InteractionParcel>& promise, const QList<PboNode*>& selection) {
                InteractionParcel data = model_->interactionPrepare(
                    selection, [&promise]() { return promise.isCanceled(); });
                promise.addResult(data);
            }, nodes);

        delete_.reset();
        cutCopyWatcher_.setFuture(future);

        return nodes;
    }

    void TreeWidget::dragStartExecute() {
        const InteractionParcel data = dragDropWatcher_.future().takeResult();
        auto* mimeData = new QMimeData;
        mimeData->setUrls(data.files());
        mimeData->setData(MIME_TYPE_PBOMAN, NodeDescriptors::serialize(data.nodes()));

        QDrag drag(this);
        drag.setMimeData(mimeData);

        emit backgroundOpStopped();

        const Qt::DropAction result = drag.exec(Qt::DropAction::CopyAction | Qt::DropAction::MoveAction);
        if (result == Qt::DropAction::MoveAction) {
            delete_.commit();
        }
    }

    void TreeWidget::copyOrCutExecute() {
        const InteractionParcel data = cutCopyWatcher_.future().takeResult();
        auto* mimeData = new QMimeData;
        mimeData->setUrls(data.files());
        mimeData->setData(MIME_TYPE_PBOMAN, NodeDescriptors::serialize(data.nodes()));

        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setMimeData(mimeData);

        emit backgroundOpStopped();
    }

    void TreeWidget::addFilesFromPbo(PboNode* target, const QMimeData* mimeData) {
        const QByteArray data = mimeData->data(MIME_TYPE_PBOMAN);
        NodeDescriptors descriptors = NodeDescriptors::deserialize(data);
        ConflictsParcel conflicts = model_->checkConflicts(target, descriptors);
        if (conflicts.hasConflicts()) {
            InsertDialog dialog(this, InsertDialog::Mode::InternalFiles, &descriptors, &conflicts);
            if (dialog.exec() == QDialog::DialogCode::Accepted) {
                model_->createNodeSet(target, descriptors, conflicts);
                delete_.commit();
            }
        } else {
            model_->createNodeSet(target, descriptors, conflicts);
            delete_.commit();
        }
    }

    void TreeWidget::addFilesFromFilesystem(const QList<QUrl>& urls) {
        NodeDescriptors files = FsCollector::collectFiles(urls);
        PboNode* item = getCurrentFolder();
        ConflictsParcel conflicts = model_->checkConflicts(item, files);
        InsertDialog dialog(this, InsertDialog::Mode::ExternalFiles, &files, &conflicts);
        if (dialog.exec() == QDialog::DialogCode::Accepted) {
            model_->createNodeSet(item, files, conflicts);
        }
    }
}
