#include "mainwindow.h"
#include <QClipboard>
#include <QDrag>
#include <QFileDialog>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QMimeData>
#include <QPoint>
#include <QtConcurrent/QtConcurrentRun>
#include "fscollector.h"
#include "ui_mainwindow.h"

using namespace pboman3;

#define MIME_TYPE_PBOMAN "application/pboman3"

MainWindow::MainWindow(QWidget* parent, PboModel2* model)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      model_(model) {
    ui_->setupUi(this);
    ui_->treeWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    delete_ = QSharedPointer<DeleteOp>(new DeleteOp(model_));
    busy_ = new BusyBar(ui_->statusBar, ui_->treeWidget);

    connect(model_, &PboModel2::onEvent, this, &MainWindow::onModelEvent);
    connect(&dragDropWatcher_, &QFutureWatcher<InteractionParcel>::finished, this, &MainWindow::dragStartExecute);
    connect(&cutCopyWatcher_, &QFutureWatcher<InteractionParcel>::finished, this, &MainWindow::copyOrCutExecute);
}

MainWindow::~MainWindow() {
    delete ui_;
}

void MainWindow::onFileOpenClick() {
    const QString fileName = QFileDialog::getOpenFileName(this, "Select a PBO", "",
                                                          "PBO Files (*.pbo);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        model_->loadFile(fileName);
    }
}

void MainWindow::onFileSaveClick() {
    model_->saveFile();
}

void MainWindow::onSelectionPasteClick() const {
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();
    if (mimeData->hasFormat(MIME_TYPE_PBOMAN)) {
        TreeWidgetItem* item = ui_->treeWidget->getSelectedFolder();
        const QByteArray data = mimeData->data(MIME_TYPE_PBOMAN);
        const NodeDescriptors descriptors = NodeDescriptors::deserialize(data);
        model_->createNodeSet(item->makePath(), descriptors);
        delete_->commit();
    } else if (mimeData->hasUrls()) {
        appendFilesToModel(mimeData->urls());
    }
}

void MainWindow::onSelectionCutClick() {
    QList<PboPath> paths = onSelectionCopyClick();
    delete_->schedule(std::move(paths));
}

QList<PboPath> MainWindow::onSelectionCopyClick() {
    busy_->start();

    QList<PboPath> paths = ui_->treeWidget->getSelectedPaths();

    const QFuture<InteractionParcel> future = QtConcurrent::run(
        [this](QPromise<InteractionParcel>& promise, const QList<PboPath>& selection) {
            InteractionParcel data = model_->interactionPrepare(selection, [&promise]() { return promise.isCanceled(); });
            promise.addResult(data);
        }, paths);

    delete_->reset();
    cutCopyWatcher_.setFuture(future);

    return paths;
}

void MainWindow::onSelectionDeleteClick() const {
    const QList<TreeWidgetItem*> selection = ui_->treeWidget->getSelectedItems();
    for (const TreeWidgetItem* item : selection) {
        model_->removeNode(item->makePath());
    }
}

void MainWindow::appendFilesToModel(const QList<QUrl>& urls) const {
    FsCollector collector;
    const NodeDescriptors files = collector.collectFiles(urls);
    //CompressDialog compressDialog(this, &files);
    //const int result = compressDialog.exec();
    //if (result == QDialog::DialogCode::Accepted) {
        TreeWidgetItem* item = ui_->treeWidget->getSelectedFolder();
        model_->createNodeSet(item->makePath(), files);
    //}
}

void MainWindow::onModelEvent(const PboModelEvent* event) const {
    if (const auto* eLoadBegin = dynamic_cast<const PboLoadBeginEvent*>(event)) {
        const QFileInfo fi(eLoadBegin->path);
        ui_->treeWidget->setNewRoot(fi.fileName());
    } else if (dynamic_cast<const PboLoadCompleteEvent*>(event)) {
        ui_->treeWidget->commitRoot();
        ui_->treeWidget->setDragDropMode(QAbstractItemView::DragDrop);
    } else if (const auto* eNodeCreated = dynamic_cast<const PboNodeCreatedEvent*>(event)) {
        ui_->treeWidget->addNewNode(*eNodeCreated->nodePath, eNodeCreated->nodeType);
    } else if (const auto* eNodeRemoved = dynamic_cast<const PboNodeRemovedEvent*>(event)) {
        ui_->treeWidget->removeNode(*eNodeRemoved->nodePath);
    }
}

void MainWindow::treeContextMenuRequested(const QPoint& point) const {
    QMenu menu;

    if (ui_->actionSelectionOpen->isEnabled()) {
        menu.addAction(ui_->actionSelectionOpen);
        menu.addSeparator();
    }

    if (ui_->actionSelectionRename->isEnabled()) {
        menu.addAction(ui_->actionSelectionRename);
        menu.addSeparator();
    }

    if (ui_->actionSelectionCopy->isEnabled()) {
        menu.addAction(ui_->actionSelectionCut);
        menu.addAction(ui_->actionSelectionCopy);
    }

    if (ui_->actionSelectionPaste->isEnabled()) {
        menu.addAction(ui_->actionSelectionPaste);
    }

    if (ui_->actionSelectionDelete->isEnabled()) {
        menu.addSeparator();
        menu.addAction(ui_->actionSelectionDelete);
    }

    menu.exec(ui_->treeWidget->mapToGlobal(point));
}

void MainWindow::treeDragStartRequested(const QList<PboPath>& paths) {
    busy_->start();

    const QFuture<InteractionParcel> future = QtConcurrent::run(
        [this](QPromise<InteractionParcel>& promise, const QList<PboPath>& selection) {
            InteractionParcel data = model_->interactionPrepare(selection, [&promise]() { return promise.isCanceled(); });
            promise.addResult(data);
        }, paths);

    delete_->schedule(paths);
    dragDropWatcher_.setFuture(future);
}

void MainWindow::treeDragDropped(const PboPath& target, const QMimeData* mimeData) {
    if (mimeData->hasFormat(MIME_TYPE_PBOMAN)) {
        const QByteArray data = mimeData->data(MIME_TYPE_PBOMAN);
        const NodeDescriptors descriptors = NodeDescriptors::deserialize(data);
        model_->createNodeSet(target, descriptors);
    } else if (mimeData->hasUrls()) {
        appendFilesToModel(mimeData->urls());
    }
}

void MainWindow::treeSelectionChanged() const {
    TreeWidgetItem* selectedFile = ui_->treeWidget->getSelectedFile();
    ui_->actionSelectionOpen->setDisabled(!selectedFile);

    const QList<QTreeWidgetItem*> items = ui_->treeWidget->selectedItems();
    const bool isSelectionValid = ui_->treeWidget->isSelectionValid();
    ui_->actionSelectionRename->setDisabled(!isSelectionValid || items.length() > 1);
    ui_->actionSelectionCopy->setDisabled(!isSelectionValid);
    ui_->actionSelectionCut->setDisabled(!isSelectionValid);
    ui_->actionSelectionDelete->setDisabled(!isSelectionValid);

    TreeWidgetItem* selectedFolder = ui_->treeWidget->getSelectedFolder();
    ui_->actionSelectionPaste->setDisabled(!selectedFolder);
}

void MainWindow::dragStartExecute() const {
    const InteractionParcel data = dragDropWatcher_.future().takeResult();
    auto* mimeData = new QMimeData;
    mimeData->setUrls(data.files());
    mimeData->setData(MIME_TYPE_PBOMAN, NodeDescriptors::serialize(data.nodes()));

    QDrag drag(ui_->treeWidget);
    drag.setMimeData(mimeData);

    busy_->stop();
    const Qt::DropAction result = drag.exec(Qt::DropAction::CopyAction | Qt::DropAction::MoveAction);
    if (result == Qt::DropAction::MoveAction) {
        delete_->commit();
    }
}

void MainWindow::copyOrCutExecute() const {
    const InteractionParcel data = cutCopyWatcher_.future().takeResult();
    auto* mimeData = new QMimeData;
    mimeData->setUrls(data.files());
    mimeData->setData(MIME_TYPE_PBOMAN, NodeDescriptors::serialize(data.nodes()));

    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setMimeData(mimeData);

    busy_->stop();
}
