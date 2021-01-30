#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>

using namespace pboman3;

MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent),
          ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(PboModel::instance, &PboModel::onEvent, this, &MainWindow::onModelEvent);

    treeModel = new TreeModel(PboModel::instance);
    ui->treeView->setModel(treeModel);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onFileOpenClick() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select a PBO", "", "PBO Files (*.pbo);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        PboModel::instance->loadFile(fileName);
    }
}

void MainWindow::onModelEvent(const PboModelEvent* event) {
    if (event->eventType == PboLoadBeginEvent::eventType) {
        onLoadBegin(dynamic_cast<const PboLoadBeginEvent*>(event));
    } else if (event->eventType == PboLoadCompleteEvent::eventType) {
        onLoadComplete(dynamic_cast<const PboLoadCompleteEvent*>(event));
    } else if (event->eventType == PboLoadFailedEvent::eventType) {
        onLoadFailed(dynamic_cast<const PboLoadFailedEvent*>(event));
    } else if (event->eventType == PboHeaderUpdatedEvent::eventType) {
        onHeaderUpdated(dynamic_cast<const PboHeaderUpdatedEvent*>(event));
    }
}

void MainWindow::onLoadBegin(const PboLoadBeginEvent* event) {
}

void MainWindow::onLoadComplete(const PboLoadCompleteEvent* event) {

}

void MainWindow::onLoadFailed(const PboLoadFailedEvent* event) {

}

void MainWindow::onHeaderUpdated(const PboHeaderUpdatedEvent* event) {

}

void MainWindow::onEntryUpdated(const PboEntryUpdatedEvent* event) {
}

