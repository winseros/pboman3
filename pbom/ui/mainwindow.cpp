#include "mainwindow.h"
#include <QDrag>
#include <QFileDialog>
#include <QFileInfo>
#include <QMimeData>
#include <QPoint>
#include <QtConcurrent/QtConcurrentRun>
#include "ui_mainwindow.h"

using namespace pboman3;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow) {
    ui_->setupUi(this);
    ui_->treeWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    connect(&model_, &PboModel2::onEvent, this, &MainWindow::onModelEvent);
    connect(ui_->treeWidget, &TreeWidget::dragStarted, this, &MainWindow::dragStarted);
    connect(ui_->treeWidget, &TreeWidget::dragDropped, this, &MainWindow::dragDropped);
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
        ui_->treeWidget->addNewNode(*eNodeCreated->path, eNodeCreated->nodeType);
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

void MainWindow::dragStarted(const QList<PboPath>& paths) {
    QFuture<InteractionData> future = QtConcurrent::run(
        [this](QPromise<InteractionData>& promise, const QList<PboPath>& pPaths) {
            InteractionData data = model_.interactionPrepare(pPaths, [&promise]() { return promise.isCanceled(); });
            promise.addResult(data);
        }, paths);

    future.waitForFinished();

    if (!future.isCanceled()) {
        const InteractionData data = future.takeResult();
        auto* mimeData = new QMimeData;
        mimeData->setUrls(data.paths);
        //mimeData->setData("application/pboman3", data.binary);

        QDrag drag(ui_->treeWidget);
        drag.setMimeData(mimeData);

        const Qt::DropAction result = drag.exec(Qt::DropAction::CopyAction | Qt::DropAction::MoveAction);
        if (result == Qt::DropAction::MoveAction) {
            for (const PboPath& path : paths) {
                model_.removeNode(path);
            }
        }
    }
}


void MainWindow::dragDropped(const PboPath& target, const QMimeData* mimeData) {
    if (mimeData->hasFormat("application/pboman3")) {
        const QByteArray data = mimeData->data("application/pboman3");
        model_.createNodeSet(target, data);
    } else if (mimeData->hasUrls()) {
        model_.createNodeSet(target, mimeData->urls());
    }
}
