#include "mainwindow.h"
#include <QClipboard>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QMimeData>
#include <QPoint>
#include <QtConcurrent/QtConcurrentRun>
#include "closedialog.h"
#include "errordialog.h"
#include "headersdialog.h"
#include "signaturedialog.h"
#include "model/diskaccessexception.h"
#include "model/pbofileformatexception.h"
#include "treewidget/treewidget.h"
#include "util/log.h"

#define LOG(...) LOGGER("ui/MainWindow", __VA_ARGS__)

namespace pboman3 {
    MainWindow::MainWindow(QWidget* parent, PboModel* model)
        : QMainWindow(parent),
          ui_(new Ui::MainWindow),
          model_(model),
          hasChanges_(false) {
        ui_->setupUi(this);

        ui_->treeWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
        ui_->treeWidget->setModel(model_);

        setupConnections();

        updateWindowTitle();
    }

    MainWindow::~MainWindow() {
        delete ui_;
    }

    void MainWindow::loadFile(const QString& fileName) {
        LOG(info, "Loading the file:", fileName)
        try {
            model_->loadFile(fileName);
            setLoaded(true);
        } catch (const PboFileFormatException& ex) {
            LOG(info, "Error when loading file - show error modal:", ex)
            UI_HANDLE_ERROR(ex)
            unloadFile();
        } catch (const DiskAccessException& ex) {
            LOG(info, "Error when loading file - show error modal:", ex)
            UI_HANDLE_ERROR(ex)
            unloadFile();
        }
    }

    void MainWindow::unloadFile() {
        setHasChanges(false);
        setLoaded(false);
        model_->unloadFile();
    }

    void MainWindow::closeEvent(QCloseEvent* event) {
        LOG(info, "A CloseEvent fired")
        if (!queryCloseUnsaved()) {
            LOG(info, "Ignoring the CloseEvent")
            event->ignore();
        }
    }

    void MainWindow::setupConnections() {
        connect(&saveWatcher_, &QFutureWatcher<int>::finished, this, &MainWindow::saveComplete);

        connect(ui_->treeWidget, &TreeWidget::backgroundOpStarted, this, [this](QFuture<void> f) { ui_->statusBar->progressShow(f); });
        connect(ui_->treeWidget, &TreeWidget::backgroundOpStopped, this, [this]() { ui_->statusBar->progressHide(); });
        connect(ui_->treeWidget, &TreeWidget::actionStateChanged, this, &MainWindow::treeActionStateChanged);
        connect(ui_->treeWidget, &TreeWidget::customContextMenuRequested, this, &MainWindow::treeContextMenuRequested);

        connect(ui_->actionFileOpen, &QAction::triggered, this, &MainWindow::onFileOpenClick);
        connect(ui_->actionFileSave, &QAction::triggered, this, &MainWindow::onFileSaveClick);
        connect(ui_->actionFileSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAsClick);
        connect(ui_->actionFileClose, &QAction::triggered, this, &MainWindow::onFileCloseClick);
        connect(ui_->actionFileExit, &QAction::triggered, this, &MainWindow::close);
        connect(ui_->actionViewHeaders, &QAction::triggered, this, &MainWindow::onViewHeadersClick);
        connect(ui_->actionViewSignature, &QAction::triggered, this, &MainWindow::onViewSignatureClick);
        connect(ui_->actionSelectionOpen, &QAction::triggered, ui_->treeWidget, &TreeWidget::selectionOpen);
        ui_->actionSelectionOpen->setShortcuts(QList<QKeySequence>({
            QKeySequence(Qt::Key_Enter),
            QKeySequence(Qt::Key_Return)
        }));
        connect(ui_->actionSelectionPaste, &QAction::triggered, ui_->treeWidget, &TreeWidget::selectionPaste);
        connect(ui_->actionSelectionCopy, &QAction::triggered, ui_->treeWidget, &TreeWidget::selectionCopy);
        connect(ui_->actionSelectionCut, &QAction::triggered, ui_->treeWidget, &TreeWidget::selectionCut);
        connect(ui_->actionSelectionRename, &QAction::triggered, ui_->treeWidget, &TreeWidget::selectionRename);
        connect(ui_->actionSelectionDelete, &QAction::triggered, ui_->treeWidget, &TreeWidget::selectionRemove);

        connect(model_, &PboModel::modelChanged, this, [this]() { setHasChanges(true); });
        connect(model_, &PboModel::loadedPathChanged, this, &MainWindow::updateWindowTitle);
    }

    void MainWindow::onFileOpenClick() {
        LOG(info, "User clicked the OpenFile button - showing dialog")
        const QString fileName = QFileDialog::getOpenFileName(this, "Select a PBO", "",
                                                              "PBO Files (*.pbo);;All Files (*.*)");
        LOG(info, "The user chose to load the file:", fileName)

        if (!fileName.isEmpty()) {
            if (queryCloseUnsaved()) {
                loadFile(fileName);
                setHasChanges(false);
            }
        }
    }

    void MainWindow::onFileSaveClick() {
        LOG(info, "User clicked the SaveFile button")
        saveFile(nullptr);
    }

    void MainWindow::onFileSaveAsClick() {
        LOG(info, "User clicked the SaveFileAs button - showing dialog")
        const QString fileName = QFileDialog::getSaveFileName(this, "Select a PBO", "",
                                                              "PBO Files (*.pbo);;All Files (*.*)");

        LOG(info, "The user chose to save the file as:", fileName)

        if (!fileName.isEmpty()) {
            saveFile(fileName);
        }
    }

    void MainWindow::onFileCloseClick() {
        LOG(info, "User clicked the CloseFile button")

        if (queryCloseUnsaved()) {
            unloadFile();
        }
    }

    void MainWindow::onViewHeadersClick() {
        LOG(info, "User clicked the ViewHeaders button")
        HeadersDialog(model_->headers(), this).exec();
    }

    void MainWindow::onViewSignatureClick() {
        LOG(info, "User clicked the ViewSignature button")
        SignatureDialog(model_->signature(), this).exec();
    }

    bool MainWindow::queryCloseUnsaved() {
        LOG(info, "Check if there are changes unsaved")
        bool proceed = true;
        if (hasChanges_) {
            LOG(info, "Yes, there are - confirm if we should proceed")
            const QFileInfo fi(model_->loadedPath());
            proceed = CloseDialog(fi, this).exec() == QDialog::DialogCode::Accepted;
        }
        LOG(info, "The result is:", proceed)
        return proceed;
    }

    void MainWindow::treeContextMenuRequested(const QPoint& point) const {
        QMenu menu;

        LOG(debug, "User opened the context menu")

        if (ui_->actionSelectionOpen->isEnabled()) {
            LOG(debug, "actionSelectionOpen - enabled")
            menu.addAction(ui_->actionSelectionOpen);
            menu.addSeparator();
        }

        if (ui_->actionSelectionRename->isEnabled()) {
            LOG(debug, "actionSelectionRename - enabled")
            menu.addAction(ui_->actionSelectionRename);
            menu.addSeparator();
        }

        if (ui_->actionSelectionCopy->isEnabled()) {
            LOG(debug, "actionSelectionCopy - enabled")
            menu.addAction(ui_->actionSelectionCut);
            menu.addAction(ui_->actionSelectionCopy);
        }

        if (ui_->actionSelectionPaste->isEnabled()) {
            LOG(debug, "actionSelectionPaste - enabled")
            menu.addAction(ui_->actionSelectionPaste);
        }

        if (ui_->actionSelectionDelete->isEnabled()) {
            LOG(debug, "actionSelectionDelete - enabled")
            menu.addSeparator();
            menu.addAction(ui_->actionSelectionDelete);
        }

        LOG(debug, "Creating the context menu")
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
        LOG(info, "Saving the file")

        const QFuture<int> future = QtConcurrent::run([this, fileName](QPromise<int>& promise) {
            model_->saveFile([&promise]() { return promise.isCanceled(); }, fileName);
            promise.addResult(0);
        });

        ui_->statusBar->progressShow(static_cast<QFuture<void>>(future));

        saveWatcher_.setFuture(future);
    }

    void MainWindow::saveComplete() {
        ui_->statusBar->progressHide();

        QFuture<int> future = saveWatcher_.future();

        if (!future.isValid()) {
            LOG(info, "File saving was cancelled - exiting")
            return;
        }

        try {
            future.takeResult();//to get exceptions rethrown
            LOG(info, "File saving is complete")
            setHasChanges(false);
        } catch (const DiskAccessException& ex) {
            LOG(info, "Error when saving - show error modal:", ex)
            UI_HANDLE_ERROR(ex)
        }
    }

    void MainWindow::setHasChanges(bool hasChanges) {
        LOG(info, "The HasChanges status set to:", hasChanges);
        ui_->actionFileSave->setEnabled(hasChanges);
        hasChanges_ = hasChanges;
        updateWindowTitle();
    }

    void MainWindow::setLoaded(bool loaded) const {
        LOG(info, "The Loaded status set to:", loaded)
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
            LOG(info, "There is no loaded file - reset window title to the default")
            setWindowTitle(TITLE);
        } else {
            const QFileInfo fi(model_->loadedPath());
            const QString title = hasChanges_
                                      ? "*" + fi.fileName() + " - " + TITLE
                                      : fi.fileName() + " - " + TITLE;
            LOG(info, "Set window title to:", title)

            setWindowTitle(title);
        }
    }
}
