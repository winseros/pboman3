#include "mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QPoint>
#include "ui_mainwindow.h"

using namespace pboman3;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow) {
    ui_->setupUi(this);

    connect(&model_, &PboModel2::onEvent, this, &MainWindow::onModelEvent);

    ui_->treeWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    treeCtrl_ = QSharedPointer<TreeControl>(new TreeControl(ui_->treeWidget));
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

void MainWindow::onModelEvent(const PboModelEvent* event) {
    if (const auto* eLoadBegin = dynamic_cast<const PboLoadBeginEvent*>(event)) {
        const QFileInfo fi(eLoadBegin->path);
        treeCtrl_->setNewRoot(fi.fileName());
    } else if (dynamic_cast<const PboLoadCompleteEvent*>(event)) {
        treeCtrl_->commitRoot();
    } else if (const auto* eNodeCreated = dynamic_cast<const PboNodeCreatedEvent*>(event)) {
        treeCtrl_->addNewNode(*eNodeCreated->path, eNodeCreated->nodeType);
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
