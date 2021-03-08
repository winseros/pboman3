#include "mainwindow.h"
#include <QDrag>
#include <QFileDialog>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QMimeData>
#include <QPoint>
#include <QtConcurrent/QtConcurrentRun>
#include "ui_mainwindow.h"

using namespace pboman3;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      busy_(nullptr),
      busyCount_(0) {
    ui_->setupUi(this);
    ui_->treeWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    connect(&model_, &PboModel2::onEvent, this, &MainWindow::onModelEvent);
    connect(ui_->treeWidget, &TreeWidget::dragStarted, this, &MainWindow::dragStartPrepare);
    connect(ui_->treeWidget, &TreeWidget::dragDropped, this, &MainWindow::dragDropped);
    connect(&interactionData_, &QFutureWatcher<InteractionData>::finished, this, &MainWindow::dragStart);
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

void MainWindow::onSelectionDeleteClick() const {
    /*const QItemSelectionModel* selection = ui_->treeView->selectionModel();
    assert(selection->hasSelection());
    QMap<const QString, const PboEntry*> affectedEntries;
    for (const QModelIndex& selected : selection->selectedIndexes()) {
        const auto* payload = static_cast<const TreeNode*>(selected.constInternalPointer());
        payload->collectEntries(affectedEntries);
    }
    for (const PboEntry* entry : affectedEntries.values()) {
        PboModel::instance->scheduleEntryDelete(entry);
    }*/
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

void MainWindow::onContextMenuRequested(const QPoint& point) {
    /*QItemSelectionModel* selection = ui_->treeView->selectionModel();
    if (selection->hasSelection()) {
        const QModelIndexList selected = selection->selectedIndexes();
        if (selected.size() == 1) {
            const auto* model = static_cast<const TreeModel*>(selected.first().constInternalPointer());
            QMenu menu;
            menu.addAction(ui_->actionSelectionOpen);
            menu.addAction(ui_->actionSelectionCopy);
            menu.addAction(ui_->actionSelectionCut);
            menu.addAction(ui_->actionSelectionDelete);
            menu.exec(ui_->treeView->mapToGlobal(point));
        } else {

        }
    }*/
}

void MainWindow::dragStartPrepare(const QList<PboPath>& paths) {
    setBusy();
    const QFuture<InteractionData> future = QtConcurrent::run(
        [this](QPromise<InteractionData>& promise, const QList<PboPath>& pPaths) {
            InteractionData data = model_.interactionPrepare(pPaths, [&promise]() { return promise.isCanceled(); });
            promise.addResult(data);
        }, paths);


    interactionData_.setFuture(future);
}

void MainWindow::dragStart() {
    const InteractionData data = interactionData_.future().takeResult();
    qDebug() << data.urls;
    auto* mimeData = new QMimeData;
    mimeData->setUrls(data.urls);
    mimeData->setData("application/pboman3", data.binary);

    QDrag drag(ui_->treeWidget);
    drag.setMimeData(mimeData);

    resetBusy();

    const Qt::DropAction result = drag.exec(Qt::DropAction::CopyAction | Qt::DropAction::MoveAction, Qt::CopyAction);
    if (result == Qt::DropAction::MoveAction) {
        for (const PboPath& path : data.nodes) {
            model_.removeNode(path);
        }
    }
}

void MainWindow::dragDropped(const PboPath& target, const QMimeData* mimeData) {
    if (mimeData->hasFormat("application/pboman3")) {
        const QByteArray data = mimeData->data("application/pboman3");
        model_.createNodeSet(target, data, nullptr);
    } else if (mimeData->hasUrls()) {
        model_.createNodeSet(target, mimeData->urls());
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
    }
    busyCount_ += 1;
    busy_->setVisible(true);
}

void MainWindow::resetBusy() {
    busy_->setVisible(busyCount_-- == 0);
}
