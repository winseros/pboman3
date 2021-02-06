#include "mainwindow.h"
#include <QFileDialog>
#include <QModelIndex>
#include <QPoint>
#include "treemodel.h"
#include "ui_mainwindow.h"

using namespace pboman3;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow) {
    ui_->setupUi(this);
    connect(PboModel::instance, &PboModel::onEvent, this, &MainWindow::onModelEvent);

    treeModel_ = QSharedPointer<TreeModel>(new TreeModel(PboModel::instance));
    ui_->treeView->setModel(treeModel_.get());
    ui_->treeView->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    connect(ui_->treeView, &QTreeView::expanded, this, &MainWindow::onTreeNodeExpanded);
    connect(ui_->treeView, &QTreeView::collapsed, this, &MainWindow::onTreeNodeCollapsed);
    connect(ui_->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::onContextMenuRequested);
}

MainWindow::~MainWindow() {
    delete ui_;
}

void MainWindow::onFileOpenClick() {
    const QString fileName = QFileDialog::getOpenFileName(this, "Select a PBO", "",
                                                          "PBO Files (*.pbo);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        PboModel::instance->loadFile(fileName);
    }
}

void MainWindow::onFileSaveClick() {
    PboModel::instance->saveFile();
}

void MainWindow::onSelectionDeleteClick() const {
    const QItemSelectionModel* selection = ui_->treeView->selectionModel();
    assert(selection->hasSelection());
    QMap<const QString, const PboEntry*> affectedEntries;
    for (const QModelIndex& selected : selection->selectedIndexes()) {
        const auto* payload = static_cast<const TreeNode*>(selected.constInternalPointer());
        payload->collectEntries(affectedEntries);
    }
    for (const PboEntry* entry : affectedEntries.values()) {
        PboModel::instance->scheduleEntryDelete(entry);
    }
}

void MainWindow::onModelEvent(const PboModelEvent* event) {
    if (const auto* evt1 = dynamic_cast<const PboLoadBeginEvent*>(event)) {
        onLoadBegin(evt1);
    } else if (const auto* evt2 = dynamic_cast<const PboLoadCompleteEvent*>(event)) {
        onLoadComplete(evt2);
    } else if (const auto* evt3 = dynamic_cast<const PboLoadFailedEvent*>(event)) {
        onLoadFailed(evt3);
    } else if (const auto* evt4 = dynamic_cast<const PboHeaderCreatedEvent*>(event)) {
        onHeaderCreated(evt4);
    }
}

void MainWindow::onContextMenuRequested(const QPoint& point) {
    QItemSelectionModel* selection = ui_->treeView->selectionModel();
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
    }
}

void MainWindow::onTreeNodeExpanded(const QModelIndex& index) const {
    auto* node = static_cast<TreeNode*>(index.internalPointer());
    node->expand(true);
}

void MainWindow::onTreeNodeCollapsed(const QModelIndex& index) const {
    auto* node = static_cast<TreeNode*>(index.internalPointer());
    node->expand(false);
}

void MainWindow::onLoadBegin(const PboLoadBeginEvent* event) {
}

void MainWindow::onLoadComplete(const PboLoadCompleteEvent* event) {

}

void MainWindow::onLoadFailed(const PboLoadFailedEvent* event) {

}

void MainWindow::onHeaderCreated(const PboHeaderCreatedEvent* event) {

}

void MainWindow::onEntryUpdated(const PboEntryCreatedEvent* event) {
}
