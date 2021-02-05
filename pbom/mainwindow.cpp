#include "mainwindow.h"
#include <QFileDialog>
#include <QModelIndex>
#include <QPoint>
#include "treemodel.h"
#include "ui_mainwindow.h"

using namespace pboman3;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(PboModel::instance, &PboModel::onEvent, this, &MainWindow::onModelEvent);

    treeModel = QSharedPointer<TreeModel>(new TreeModel(PboModel::instance));
    ui->treeView->setModel(treeModel.get());
    ui->treeView->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    connect(ui->treeView, &QTreeView::expanded, treeModel.get(), &TreeModel::viewExpanded);
    connect(ui->treeView, &QTreeView::collapsed, treeModel.get(), &TreeModel::viewCollapsed);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::onContextMenuRequested);
}

MainWindow::~MainWindow() {
    delete ui;
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
    const QItemSelectionModel* selection = ui->treeView->selectionModel();
    assert(selection->hasSelection());
    QMap<const QString, const PboEntry*> affectedEntries;
    for (const QModelIndex& selected: selection->selectedIndexes()) {
        const auto* payload = static_cast<const TreeNode*>(selected.constInternalPointer());
        payload->collectEntries(affectedEntries);
    }
    for (const PboEntry* entry: affectedEntries.values()) {
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
    QItemSelectionModel* selection = ui->treeView->selectionModel();
    if (selection->hasSelection()) {
        const QModelIndexList selected = selection->selectedIndexes();
        if (selected.size() == 1) {
            const auto* model = static_cast<const TreeModel*>(selected.first().constInternalPointer());
            QMenu menu;
            menu.addAction(ui->actionSelectionOpen);
            menu.addAction(ui->actionSelectionCopy);
            menu.addAction(ui->actionSelectionCut);
            menu.addAction(ui->actionSelectionDelete);
            menu.exec(ui->treeView->mapToGlobal(point));
        } else {
            
        }
    }
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
