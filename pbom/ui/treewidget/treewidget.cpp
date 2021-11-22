#include "treewidget.h"
#include <QClipboard>
#include <QMimeData>
#include <QtConcurrent/QtConcurrentRun>
#include <QDrag>
#include "ui/fscollector.h"
#include "ui/insertdialog.h"
#include "exception.h"
#include <QDesktopServices>
#include "io/diskaccessexception.h"
#include "ui/errordialog.h"
#include "ui/win32/win32fileviewer.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/treewidget/TreeWidget", __VA_ARGS__)

namespace pboman3::ui {
#define MIME_TYPE_PBOMAN "application/pboman3"

    TreeWidget::TreeWidget(QWidget* parent)
        : TreeWidgetBase(parent),
          model_(nullptr),
          actionState_() {

        connect(&dragDropWatcher_, &QFutureWatcher<InteractionParcel>::finished, this, &TreeWidget::dragStartExecute);
        connect(&cutCopyWatcher_, &QFutureWatcher<InteractionParcel>::finished, this, &TreeWidget::copyOrCutExecute);
        connect(&openWatcher_, &QFutureWatcher<QString>::finished, this, &TreeWidget::openExecute);
        connect(&extractWatcher_, &QFutureWatcher<int>::finished, this, &TreeWidget::extractExecute);
        connect(this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::onSelectionChanged);
        connect(this, &TreeWidget::doubleClicked, this, &TreeWidget::onDoubleClicked);
    }

    void TreeWidget::selectionOpen() {
        if (!actionState_.canOpen)
            throw InvalidOperationException("Open action is not available");

        LOG(info, "Opening the selected item")

        const auto* selected = dynamic_cast<TreeWidgetItem*>(currentItem());

        const QFuture<QString> future = QtConcurrent::run(
            [this](QPromise<QString>& promise, const PboNode* node) {
                LOG(info, "Extracting the node:", *node)
                QString filePath = model_->execPrepare(node, [&promise]() { return promise.isCanceled(); });
                LOG(info, "Extracted the node as:", filePath)
                promise.addResult(filePath);
            }, selected->node());

        emit backgroundOpStarted(static_cast<QFuture<void>>(future));

        openWatcher_.setFuture(future);
    }

    void TreeWidget::selectionExtract(const QString& dir, const PboNode* relativeTo) {
        if (!actionState_.canExtract)
            throw InvalidOperationException("Extract action is not available");

        assert(relativeTo);

        LOG(info, "Extracting selected nodes to: ", dir, "relative to ", relativeTo->title())

        QList<PboNode*> selected = getSelectedHierarchies();
        LOG(info, selected.count(), "hierarchy items selected")

        const QFuture<int> future = QtConcurrent::run(
            [this](QPromise<int>& promise,
                   const QString& pDir,
                   const PboNode* pRelativeTo,
                   const QList<PboNode*>& nodes) {
                LOG(info, "Unpack nodes begin")
                model_->unpackNodesTo(QDir(pDir), pRelativeTo, nodes, [&promise]() { return promise.isCanceled(); });
                LOG(info, "Unpack nodes complete")
                promise.addResult(0);
            }, dir, relativeTo, std::move(selected));

        emit backgroundOpStarted(static_cast<QFuture<void>>(future));

        extractWatcher_.setFuture(future);
    }

    void TreeWidget::selectionCopy() {
        if (!actionState_.canCopy)
            throw InvalidOperationException("Copy action is not available");

        LOG(info, "Copy selected items")

        selectionCopyImpl();
    }

    void TreeWidget::selectionCut() {
        if (!actionState_.canCut)
            throw InvalidOperationException("Cut action is not available");

        LOG(info, "Cut selected items")

        QList<PboNode*> nodes = selectionCopyImpl();
        delete_.schedule(std::move(nodes));
    }

    void TreeWidget::selectionPaste() {
        if (!actionState_.canPaste)
            throw InvalidOperationException("Paste action is not available");

        LOG(info, "Paste to the selected item")

        if (PboNode* item = getCurrentFolder()) {
            LOG(info, "The selected item is:", *item)

            const QClipboard* clipboard = QGuiApplication::clipboard();
            const QMimeData* mimeData = clipboard->mimeData();
            if (mimeData->hasFormat(MIME_TYPE_PBOMAN)) {
                LOG(info, "Clipboard contained data in internal format")
                addFilesFromPbo(item, mimeData);
            } else if (mimeData->hasUrls()) {
                LOG(info, "Clipboard contained URLs")
                addFilesFromFilesystem(mimeData->urls());
            }
        }
    }

    void TreeWidget::selectionRemove() const {
        if (!actionState_.canRemove)
            throw InvalidOperationException("Remove action is not available");

        LOG(info, "Remove the selected items")

        const QList<PboNode*> selection = getSelectedHierarchies();
        for (PboNode* item : selection) {
            LOG(debug, "Remove item:", *item)
            item->removeFromHierarchy();
        }
    }

    void TreeWidget::selectionRename() const {
        if (!actionState_.canRename)
            throw InvalidOperationException("Rename action is not available");

        LOG(info, "Rename the current item")

        const auto* selected = dynamic_cast<TreeWidgetItem*>(currentItem());

        selected->rename();
    }

    void TreeWidget::setWidgetModel(PboModel* model) {
        LOG(info, "Using the new model")
        model_ = model;
    }

    void TreeWidget::dragStarted(const QList<PboNode*>& items) {
        LOG(info, "Drag operation has started for", items.length(), "items")

        const QFuture<InteractionParcel> future = QtConcurrent::run(
            [this](QPromise<InteractionParcel>& promise, const QList<PboNode*>& selection) {
                LOG(info, "Extracting the selected items to the drive")
                InteractionParcel data = model_->interactionPrepare(
                    selection, [&promise]() { return promise.isCanceled(); });
                LOG(info, "Extraction complete:", data)
                promise.addResult(data);
            }, items);

        emit backgroundOpStarted(static_cast<QFuture<void>>(future));

        delete_.schedule(items);
        dragDropWatcher_.setFuture(future);
    }

    void TreeWidget::dragDropped(PboNode* target, const QMimeData* mimeData) {
        LOG(info, "Drag drop to the node:", *target)
        if (mimeData->hasFormat(MIME_TYPE_PBOMAN)) {
            LOG(info, "Clipboard contained data in internal format")
            addFilesFromPbo(target, mimeData);
        } else if (mimeData->hasUrls()) {
            LOG(info, "Clipboard contained URLs")
            addFilesFromFilesystem(mimeData->urls());
        }
    }

    void TreeWidget::onDoubleClicked() {
        LOG(info, "Double click at the item")
        auto* current = dynamic_cast<TreeWidgetItem*>(currentItem());
        if (current->node()->nodeType() == PboNodeType::File) {
            LOG(info, "The selected node was a file - open it")
            selectionOpen();
        } else {
            LOG(info, "The selected node was a folder - toggle it")
            current->setExpanded(!current->isExpanded());
        }
    }

    void TreeWidget::onSelectionChanged() {
        LOG(debug, "Selection changed")

        actionState_.canOpen = getCurrentFile();
        actionState_.canRename = !!currentItem();
        actionState_.canExtract = actionState_.canRename;

        auto canCopy = [this]() {
            QList<PboNode*> nodes = getSelectedHierarchies();
            if (!nodes.count())
                return false;
            const bool rootSelected = !std::any_of(nodes.begin(), nodes.end(), [](const PboNode* n) {
                return !n->parentNode();
            });
            return rootSelected;
        };

        actionState_.canCopy = canCopy();
        actionState_.canCut = actionState_.canCopy;
        actionState_.canRemove = actionState_.canCopy;

        actionState_.canPaste = getCurrentFolder();

        LOG(debug, "Action state is:", actionState_)

        emit actionStateChanged(actionState_);
    }

    QList<PboNode*> TreeWidget::selectionCopyImpl() {
        LOG(info, "Perform Cut/Copy operation")

        QList<PboNode*> nodes = getSelectedHierarchies();
        LOG(info, nodes.count(), "hierarchy items selected")

        const QFuture<InteractionParcel> future = QtConcurrent::run(
            [this](QPromise<InteractionParcel>& promise, const QList<PboNode*>& selection) {
                LOG(info, "Extracting the selected items to the drive")
                InteractionParcel data = model_->interactionPrepare(
                    selection, [&promise]() { return promise.isCanceled(); });
                LOG(info, "Extraction complete:", data)
                promise.addResult(data);
            }, nodes);

        emit backgroundOpStarted(static_cast<QFuture<void>>(future));

        delete_.reset();
        cutCopyWatcher_.setFuture(future);

        return nodes;
    }

    void TreeWidget::openExecute() {
        emit backgroundOpStopped();

        QFuture<QString> future = openWatcher_.future();

        if (!future.isValid()) {
            LOG(info, "The Open operation was cancelled - exiting")
            return;
        }

        LOG(info, "Executing the Open operation")

        try {
            const QString path = future.takeResult();
            LOG(info, "The open operation result is:", path)
            Win32FileViewer().previewFile(path);
        } catch (const DiskAccessException& ex) {
            LOG(info, "Error when running sync - show error modal:", ex)
            UI_HANDLE_ERROR(ex)
        } catch (const Win32FileViewerException& ex) {
            UI_HANDLE_ERROR(ex)
        }
    }

    void TreeWidget::extractExecute() {
        emit backgroundOpStopped();

        QFuture<int> future = extractWatcher_.future();

        if (!future.isValid()) {
            LOG(info, "The Extract operation was cancelled - exiting")
            return;
        }

        try {
            future.takeResult(); //to catch errors, if any
            LOG(info, "The Extract operation complete")
        } catch (const DiskAccessException& ex) {
            UI_HANDLE_ERROR(ex)
        }
    }

    void TreeWidget::dragStartExecute() {
        emit backgroundOpStopped();

        QFuture<InteractionParcel> future = dragDropWatcher_.future();

        if (!future.isValid()) {
            LOG(info, "The dragStart operation was cancelled - exiting")
            return;
        }

        LOG(info, "Executing the dragStart operation")

        InteractionParcel data;
        try {
            data = future.takeResult();
            LOG(info, "The interaction parsel is:", data)
        } catch (const DiskAccessException& ex) {
            LOG(info, "Error when running sync - show error modal:", ex)
            UI_HANDLE_ERROR_RET(ex)
        }

        auto* mimeData = new QMimeData;
        mimeData->setUrls(data.files());
        mimeData->setData(MIME_TYPE_PBOMAN, NodeDescriptors::serialize(data.nodes()));

        QDrag drag(this);
        drag.setMimeData(mimeData);

        LOG(info, "Begin the system drag-drop operation")
        const Qt::DropAction result = drag.exec(Qt::DropAction::CopyAction | Qt::DropAction::MoveAction);

        LOG(info, "The operation finished with the result:", result)
        if (result == Qt::DropAction::MoveAction) {
            delete_.commit();
        }
    }

    void TreeWidget::copyOrCutExecute() {
        emit backgroundOpStopped();

        QFuture<InteractionParcel> future = cutCopyWatcher_.future();

        if (!future.isValid()) {
            LOG(info, "The Cut/Copy operation was cancelled - exiting")
            return;
        }

        LOG(info, "Executing the Cut/Copy operation")

        InteractionParcel data;
        try {
            data = future.takeResult();
            LOG(info, "The interaction parsel is:", data)
        } catch (const DiskAccessException& ex) {
            LOG(info, "Error when running sync - show error modal:", ex)
            UI_HANDLE_ERROR_RET(ex)
        }

        auto* mimeData = new QMimeData;
        mimeData->setUrls(data.files());
        mimeData->setData(MIME_TYPE_PBOMAN, NodeDescriptors::serialize(data.nodes()));

        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setMimeData(mimeData);
    }

    void TreeWidget::addFilesFromPbo(PboNode* target, const QMimeData* mimeData) {
        LOG(info, "Add files that are already a part of the PBO")
        const QByteArray data = mimeData->data(MIME_TYPE_PBOMAN);

        NodeDescriptors descriptors = NodeDescriptors::deserialize(data);
        LOG(debug, "Deserialized descriptors:", descriptors)

        ConflictsParcel conflicts = model_->checkConflicts(target, descriptors);
        LOG(debug, "The result of conflicts check:", conflicts)

        if (conflicts.hasConflicts()) {
            LOG(info, "There were conflicts - running the resolution dialog")
            InsertDialog dialog(this, InsertDialog::Mode::InternalFiles, &descriptors, &conflicts);
            if (dialog.exec() == QDialog::DialogCode::Accepted) {
                LOG(info, "The user accepted the conflicts dialog")
                model_->createNodeSet(target, descriptors, conflicts);
                delete_.commit();
            }
        } else {
            LOG(info, "There were no conflicts - adding the nodes as is")
            model_->createNodeSet(target, descriptors, conflicts);
            delete_.commit();
        }
    }

    void TreeWidget::addFilesFromFilesystem(const QList<QUrl>& urls) {
        LOG(info, "Add files from the file system:", urls)

        NodeDescriptors files;
        try {
            files = FsCollector::collectFiles(urls);
            LOG(debug, "Collected descriptors:", files)
        } catch (const DiskAccessException& ex) {
            LOG(info, "Error when collecting - show error modal:", ex)
            UI_HANDLE_ERROR_RET(ex)
        }

        PboNode* item = getCurrentFolder();
        LOG(info, "Selected node is:", *item)

        ConflictsParcel conflicts = model_->checkConflicts(item, files);
        LOG(debug, "The result of conflicts check:", conflicts)

        InsertDialog dialog(this, InsertDialog::Mode::ExternalFiles, &files, &conflicts);
        if (dialog.exec() == QDialog::DialogCode::Accepted) {
            LOG(info, "The user accepted the file insert dialog")
            model_->createNodeSet(item, files, conflicts);
        }
    }
}
