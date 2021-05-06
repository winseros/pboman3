#include "mainwindow.h"
#include <QClipboard>
#include <QDrag>
#include <QFileDialog>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QMimeData>
#include <QPoint>
#include <QtConcurrent/QtConcurrentRun>
#include "compressdialog.h"
#include "fscollector.h"
#include "ui_mainwindow.h"
#include "model/filesystemfiles.h"

using namespace pboman3;

#define MIME_TYPE_PBOMAN "application/pboman3"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      busy_(nullptr),
      busyCount_(0) {
    ui_->setupUi(this);
    ui_->treeWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    connect(&model_, &PboModel2::onEvent, this, &MainWindow::onModelEvent);
    connect(&dragDropWatcher_, &QFutureWatcher<InteractionData>::finished, this, &MainWindow::dragStartExecute);
    connect(&cutCopyWatcher_, &QFutureWatcher<InteractionData>::finished, this, &MainWindow::copyOrCutExecute);
}

MainWindow::~MainWindow() {
    delete ui_;
}

void MainWindow::onFileOpenClick() {
    const QString fileName = QFileDialog::getOpenFileName(this, "Select a PBO", "",
                                                          "PBO Files (*.pbo);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        model_.loadFile(fileName);
    }
}

void MainWindow::onFileSaveClick() {
    model_.saveFile();
}

void MainWindow::onSelectionPasteClick() {
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();
    if (mimeData->hasFormat(MIME_TYPE_PBOMAN)) {
        TreeWidgetItem* item = ui_->treeWidget->getSelectedFolder();
        const QByteArray data = mimeData->data(MIME_TYPE_PBOMAN);
        model_.createNodeSet(item->makePath(), data, [this](TreeConflicts&) {});
    } else if (mimeData->hasUrls()) {
        appendFilesToModel(mimeData->urls());
    }
    for (const PboPath& path : pendingCutOp_) {
        model_.removeNode(path);
    }
    pendingCutOp_.clear();
}

void MainWindow::onSelectionCutClick() {
    pendingCutOp_ = onSelectionCopyClick();
}

QList<PboPath> MainWindow::onSelectionCopyClick() {
    setBusy();

    QList<PboPath> paths = ui_->treeWidget->getSelectedPaths();

    const QFuture<InteractionData> future = QtConcurrent::run(
        [this](QPromise<InteractionData>& promise, const QList<PboPath>& pPaths) {
            InteractionData data = model_.interactionPrepare(pPaths, [&promise]() { return promise.isCanceled(); });
            promise.addResult(data);
        }, paths);

    pendingCutOp_.clear();
    cutCopyWatcher_.setFuture(future);

    return paths;
}

void MainWindow::onSelectionDeleteClick() const {
    const QList<TreeWidgetItem*> selection = ui_->treeWidget->getSelectedItems();
    for (const TreeWidgetItem* item : selection) {
        model_.removeNode(item->makePath());
    }
}

void MainWindow::appendFilesToModel(const QList<QUrl>& urls) {
    FsCollector collector;
    const FilesystemFiles files = collector.collectFiles(urls);
    CompressDialog compressDialog(this, &files);
    const int result = compressDialog.exec();
    if (result == QDialog::DialogCode::Accepted) {
        TreeWidgetItem* item = ui_->treeWidget->getSelectedFolder();
        model_.createNodeSet(item->makePath(), files, [this](TreeConflicts) {});
    }
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

void MainWindow::setBusy() {
    if (!busy_) {
        busy_ = new QProgressBar();
        busy_->setTextVisible(false);
        busy_->setMinimum(0);
        busy_->setMaximum(0);
        busy_->setFixedHeight(15);
        ui_->statusBar->addWidget(busy_, 1);
        ui_->treeWidget->setDisabled(true);
    }
    busyCount_ += 1;
    busy_->setVisible(true);
}

void MainWindow::resetBusy() {
    busyCount_ -= 1;
    busy_->setVisible(busyCount_ != 0);
    ui_->treeWidget->setDisabled(busyCount_ != 0);
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
    setBusy();

    const QFuture<InteractionData> future = QtConcurrent::run(
        [this](QPromise<InteractionData>& promise, const QList<PboPath>& pPaths) {
            InteractionData data = model_.interactionPrepare(pPaths, [&promise]() { return promise.isCanceled(); });
            promise.addResult(data);
        }, paths);

    pendingCutOp_.clear();
    dragDropWatcher_.setFuture(future);
}

void MainWindow::treeDragDropped(const PboPath& target, const QMimeData* mimeData) {
    if (mimeData->hasFormat(MIME_TYPE_PBOMAN)) {
        const QByteArray data = mimeData->data(MIME_TYPE_PBOMAN);
        model_.createNodeSet(target, data, [this](TreeConflicts) {});
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

void MainWindow::dragStartExecute() {
    const InteractionData data = dragDropWatcher_.future().takeResult();
    auto* mimeData = new QMimeData;
    mimeData->setUrls(data.urls);
    mimeData->setData(MIME_TYPE_PBOMAN, data.binary);

    QDrag drag(ui_->treeWidget);
    drag.setMimeData(mimeData);

    resetBusy();
    const Qt::DropAction result = drag.exec(Qt::DropAction::CopyAction | Qt::DropAction::MoveAction);
    if (result == Qt::DropAction::MoveAction) {
        for (const PboPath& path : data.nodes) {
            model_.removeNode(path);
        }
    }
}

void MainWindow::copyOrCutExecute() {
    const InteractionData data = cutCopyWatcher_.future().takeResult();
    auto* mimeData = new QMimeData;
    mimeData->setUrls(data.urls);
    mimeData->setData(MIME_TYPE_PBOMAN, data.binary);

    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setMimeData(mimeData);

    resetBusy();
}
