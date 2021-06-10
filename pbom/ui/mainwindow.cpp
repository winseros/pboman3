#include "mainwindow.h"
#include <QClipboard>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QMimeData>
#include <QPoint>
#include <QtConcurrent/QtConcurrentRun>

#include "closedialog.h"
#include "headersdialog.h"
#include "insertdialog.h"
#include "signaturedialog.h"
#include "ui_mainwindow.h"
#include "treewidget/treewidget.h"

namespace pboman3 {
    MainWindow::MainWindow(QWidget* parent, PboModel* model)
        : QMainWindow(parent),
          ui_(new Ui::MainWindow),
          model_(model),
          hasChanges_(false) {
        ui_->setupUi(this);

        ui_->treeWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
        ui_->treeWidget->setModel(model_);

        busy_ = new BusyBar(ui_->treeWidget);
        ui_->statusBar->addWidget(busy_, 1);

        setupConnections();

        updateWindowTitle();
    }

    MainWindow::~MainWindow() {
        delete ui_;
    }

    void MainWindow::loadFile(const QString& fileName) const {
        model_->loadFile(fileName);
        setLoaded(true);
    }

    void MainWindow::setupConnections() {
        connect(&saveWatcher_, &QFutureWatcher<void>::finished, this, &MainWindow::saveComplete);

        connect(ui_->treeWidget, &TreeWidget::backgroundOpStarted, this, [this]() { busy_->start(); });
        connect(ui_->treeWidget, &TreeWidget::backgroundOpStopped, this, [this]() { busy_->stop(); });
        connect(ui_->treeWidget, &TreeWidget::actionStateChanged, this, &MainWindow::treeActionStateChanged);
        connect(ui_->treeWidget, &TreeWidget::customContextMenuRequested, this, &MainWindow::treeContextMenuRequested);

        connect(ui_->actionFileOpen, &QAction::triggered, this, &MainWindow::onFileOpenClick);
        connect(ui_->actionFileSave, &QAction::triggered, this, &MainWindow::onFileSaveClick);
        connect(ui_->actionFileSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAsClick);
        connect(ui_->actionFileClose, &QAction::triggered, this, &MainWindow::onFileCloseClick);
        connect(ui_->actionViewHeaders, &QAction::triggered, this, &MainWindow::onViewHeadersClick);
        connect(ui_->actionViewSignature, &QAction::triggered, this, &MainWindow::onViewSignatureClick);
        connect(ui_->actionSelectionPaste, &QAction::triggered, ui_->treeWidget, &TreeWidget::selectionPaste);
        connect(ui_->actionSelectionCopy, &QAction::triggered, ui_->treeWidget, &TreeWidget::selectionCopy);
        connect(ui_->actionSelectionCut, &QAction::triggered, ui_->treeWidget, &TreeWidget::selectionCut);
        connect(ui_->actionSelectionRename, &QAction::triggered, ui_->treeWidget, &TreeWidget::selectionRename);
        connect(ui_->actionSelectionDelete, &QAction::triggered, ui_->treeWidget, &TreeWidget::selectionRemove);

        connect(model_, &PboModel::modelChanged, this, [this]() { setHasChanges(true); });
        connect(model_, &PboModel::loadedPathChanged, this, &MainWindow::updateWindowTitle);
    }

    void MainWindow::onFileOpenClick() {
        const QString fileName = QFileDialog::getOpenFileName(this, "Select a PBO", "",
                                                              "PBO Files (*.pbo);;All Files (*.*)");
        if (!fileName.isEmpty()) {
            bool proceed = true;
            if (hasChanges_) {
                const QFileInfo fi(model_->loadedPath());
                proceed = CloseDialog(fi, this).exec() == QDialog::DialogCode::Accepted;
            }
            if (proceed) {
                loadFile(fileName);
                setHasChanges(false);
            }
        }
    }

    void MainWindow::onFileSaveClick() {
        saveFile(nullptr);
    }

    void MainWindow::onFileSaveAsClick() {
        const QString fileName = QFileDialog::getSaveFileName(this, "Select a PBO", "",
                                                              "PBO Files (*.pbo);;All Files (*.*)");
        if (!fileName.isEmpty()) {
            saveFile(fileName);
        }
    }

    void MainWindow::onFileCloseClick() {
        bool proceed = true;
        if (hasChanges_) {
            const QFileInfo fi(model_->loadedPath());
            proceed = CloseDialog(fi, this).exec() == QDialog::DialogCode::Accepted;
        }
        if (proceed) {
            setHasChanges(false);
            setLoaded(false);
            model_->unloadFile();
        }
    }

    void MainWindow::onViewHeadersClick() {
        HeadersDialog(model_->headers(), this).exec();
    }

    void MainWindow::onViewSignatureClick() {
        SignatureDialog(model_->signature(), this).exec();
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

    void MainWindow::treeActionStateChanged(const TreeWidget::ActionState& state) const {
        ui_->actionSelectionOpen->setEnabled(state.canOpen);
        ui_->actionSelectionRename->setEnabled(state.canRename);
        ui_->actionSelectionCopy->setEnabled(state.canCopy);
        ui_->actionSelectionCut->setEnabled(state.canCut);
        ui_->actionSelectionPaste->setEnabled(state.canPaste);
        ui_->actionSelectionDelete->setEnabled(state.canRemove);
    }

    void MainWindow::saveFile(const QString& fileName) {
        busy_->start();

        const QFuture<void> future = QtConcurrent::run([this, fileName](QPromise<void>& promise) {
            model_->saveFile([&promise]() { return promise.isCanceled(); }, fileName);
        });

        saveWatcher_.setFuture(future);
    }

    void MainWindow::saveComplete() {
        setHasChanges(false);
        busy_->stop();
    }

    void MainWindow::setHasChanges(bool hasChanges) {
        ui_->actionFileSave->setEnabled(hasChanges);
        hasChanges_ = hasChanges;
        updateWindowTitle();
    }

    void MainWindow::setLoaded(bool loaded) const {
        if (loaded) {
            ui_->treeWidget->setRoot(model_->rootEntry());
            ui_->treeWidget->setDragDropMode(QAbstractItemView::DragDrop);
        } else {
            ui_->treeWidget->resetRoot();
            ui_->treeWidget->setDragDropMode(QAbstractItemView::NoDragDrop);

            ui_->actionSelectionOpen->setEnabled(false);
            ui_->actionSelectionCopy->setEnabled(false);
            ui_->actionSelectionCut->setEnabled(false);
            ui_->actionSelectionPaste->setEnabled(false);
            ui_->actionSelectionDelete->setEnabled(false);
            ui_->actionSelectionRename->setEnabled(false);
        }

        ui_->actionFileSaveAs->setEnabled(loaded);
        ui_->actionFileClose->setEnabled(loaded);
        ui_->actionViewHeaders->setEnabled(loaded);
        ui_->actionViewSignature->setEnabled(loaded);
    }

    void MainWindow::updateWindowTitle() {
#define TITLE "PBO Manager 3.0"
        if (model_->loadedPath().isNull()) {
            setWindowTitle(TITLE);
        } else {
            const QFileInfo fi(model_->loadedPath());
            setWindowTitle(hasChanges_
                               ? "*" + fi.fileName() + " - " + TITLE
                               : fi.fileName() + " - " + TITLE);
        }
    }
}
