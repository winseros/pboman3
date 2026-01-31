#include "mainwindow.h"
#include <QClipboard>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QPoint>
#include <QtConcurrent/QtConcurrentRun>
#include "aboutdialog.h"
#include "closedialog.h"
#include "errordialog.h"
#include "headersdialog.h"
#include "signaturedialog.h"
#include "updatesdialog.h"
#include "settingsdialog.h"
#include "ui_mainwindow.h"
#include "domain/func.h"
#include "io/diskaccessexception.h"
#include "io/pbofileformatexception.h"
#include "io/bb/sanitizedpath.h"
#include "model/pbomodel.h"
#include "treewidget/treewidget.h"
#include "util/log.h"
#include "util/filenames.h"
#include "io/bb/sanitizedstring.h"

#define LOG(...) LOGGER("ui/MainWindow", __VA_ARGS__)

namespace pboman3::ui {
    MainWindow::MainWindow(QWidget* parent, model::PboModel* model)
        : QMainWindow(parent),
          ui_(new Ui::MainWindow),
          model_(model),
          hasChanges_(false) {
        ui_->setupUi(this);

        ui_->treeWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
        ui_->treeWidget->setWidgetModel(model_);

        setupConnections();

        updateWindowTitle();
    }

    MainWindow::~MainWindow() {
        delete ui_;
    }

    void MainWindow::loadFile(const QString& fileName) {
        if (model_->isLoaded())
            unloadFile();

        const QFuture<int> future = QtConcurrent::run([this](QPromise<int>& promise, const QString& fname) {
            LOG(info, "Loading the file:", fname)
            model_->loadFile(fname);
            promise.addResult(0);
        }, fileName);

        setIsLoading(static_cast<QFuture<void>>(future), false);
        loadWatcher_.setFuture(future);
    }

    void MainWindow::loadComplete() {
        resetIsLoading();

        QFuture<int> future = loadWatcher_.future();

        if (!future.isValid()) {
            LOG(info, "File loading was cancelled - exiting")
            return;
        }

        try {
            future.takeResult(); //to get exceptions rethrown
            LOG(info, "File loading is complete")
            setHasChanges(false);
            ui_->treeWidget->selectContainerItem();
        } catch (const PboFileFormatException& ex) {
            LOG(info, "Error when loading file - show error modal:", ex)
            UI_HANDLE_ERROR(ex)
        } catch (const DiskAccessException& ex) {
            LOG(info, "Error when loading file - show error modal:", ex)
            UI_HANDLE_ERROR(ex)
        }
    }

    void MainWindow::unloadFile() {
        LOG(info, "Unloading the current file")
        setHasChanges(false);
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
        connect(&loadWatcher_, &QFutureWatcher<int>::finished, this, &MainWindow::loadComplete);
        connect(&saveWatcher_, &QFutureWatcher<int>::finished, this, &MainWindow::saveComplete);

        connect(ui_->treeWidget, &TreeWidget::backgroundOpStarted, this, [this](QFuture<void> f) {
            ui_->treeWidget->setEnabled(false);
            ui_->statusBar->progressShow(std::move(f), true);
        });
        connect(ui_->treeWidget, &TreeWidget::backgroundOpStopped, this, [this]() {
            ui_->treeWidget->setEnabled(true);
            ui_->statusBar->progressHide();
        });
        connect(ui_->treeWidget, &TreeWidget::actionStateChanged, this, &MainWindow::treeActionStateChanged);
        connect(ui_->treeWidget, &TreeWidget::customContextMenuRequested, this, &MainWindow::treeContextMenuRequested);

        connect(ui_->actionFileOpen, &QAction::triggered, this, &MainWindow::onFileOpenClick);
        connect(ui_->actionFileSave, &QAction::triggered, this, &MainWindow::onFileSaveClick);
        connect(ui_->actionFileSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAsClick);
        connect(ui_->actionFileClose, &QAction::triggered, this, &MainWindow::onFileCloseClick);
        connect(ui_->actionFileExit, &QAction::triggered, this, &MainWindow::close);
        connect(ui_->actionViewHeaders, &QAction::triggered, this, &MainWindow::onViewHeadersClick);
        connect(ui_->actionViewSignature, &QAction::triggered, this, &MainWindow::onViewSignatureClick);

        connect(ui_->actionSelectionExtractTo, &QAction::triggered, this, &MainWindow::selectionExtractToClick);
        connect(ui_->actionSelectionExtractFolder, &QAction::triggered, this, &MainWindow::selectionExtractFolderClick);
        connect(ui_->actionSelectionExtractContainer, &QAction::triggered, this,
                &MainWindow::selectionExtractContainerClick);
        connect(ui_->actionSelectionExtractToPrefix, &QAction::triggered, this, &MainWindow::selectionExtractToPrefixClick);
        connect(ui_->actionExportPboJson, &QAction::triggered, this, &MainWindow::exportPboJsonClick);

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

        connect(ui_->actionSettings, &QAction::triggered, [this](){ SettingsDialog(this).exec(); });

        connect(ui_->actionHelpAbout, &QAction::triggered, [this]() { AboutDialog(this).exec(); });
        connect(ui_->actionCheckUpdates, &QAction::triggered, [this]() { UpdatesDialog(this).exec(); });

        connect(model_, &PboModel::modelChanged, this, [this]() { setHasChanges(true); });
        connect(model_, &PboModel::loadedPathChanged, this, &MainWindow::updateWindowTitle);
        connect(model_, &PboModel::loadedStatusChanged, this, &MainWindow::updateLoadedStatus);
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
        HeadersDialog(model_->document()->headers(), this).exec();
    }

    void MainWindow::onViewSignatureClick() {
        LOG(info, "User clicked the ViewSignature button")
        SignatureDialog(&model_->document()->signature(), this).exec();
    }

    void MainWindow::selectionExtractToClick() {
        LOG(info, "User clicked the ExtractTo button - showing dialog")
        QString folderPath = QFileDialog::getExistingDirectory(this, "Select the directory");

        if (!folderPath.isEmpty()) {
            LOG(info, "User selected the path:", folderPath)
            const PboNode* selectionRoot = ui_->treeWidget->getSelectionRoot();
            if (selectionRoot->nodeType() == PboNodeType::File) {
                selectionRoot = selectionRoot->parentNode();
            } else {
                QString folderName = selectionRoot->title();
                if (selectionRoot->nodeType() == PboNodeType::Container)
                    folderName = FileNames::getFileNameWithoutExtension(folderName);
                const QDir dir(folderPath);
                folderPath = dir.filePath(folderName);
                if (!QDir(dir.filePath(folderName)).exists() && !dir.mkdir(folderName)) {
                    LOG(critical, "Could not create the dir:", folderPath)
                    ErrorDialog("Could not create the folder<br><br><b>" + folderPath + "<b>").exec();
                    return;
                }
            }

            LOG(info, "Extracting to:", folderPath)
            ui_->treeWidget->selectionExtract(folderPath, selectionRoot);
        }
    }

    void MainWindow::selectionExtractFolderClick() const {
        LOG(info, "User clicked the ExtractToFolder button")

        const QDir dir = QFileInfo(model_->loadedPath()).dir();
        const PboNode* selectionRoot = ui_->treeWidget->getSelectionRoot();

        QString folderPath;
        if (selectionRoot->nodeType() == PboNodeType::File) {
            selectionRoot = selectionRoot->parentNode();
            folderPath = dir.absolutePath();
        } else {
            const auto folderName = static_cast<QString>(SanitizedString(selectionRoot->title()));
            folderPath = dir.filePath(folderName);
            if (!QDir(dir.filePath(folderName)).exists() && !dir.mkdir(folderName)) {
                LOG(critical, "Could not create the dir:", folderPath)
                ErrorDialog("Could not create the folder<br><br><b>" + folderPath + "<b>").exec();
                return;
            }
        }

        LOG(info, "Extracting to:", folderPath)
        ui_->treeWidget->selectionExtract(folderPath, selectionRoot);
    }

    void MainWindow::selectionExtractContainerClick() const {
        LOG(info, "User clicked the ExtractToContainer button")
        const QDir dir = QFileInfo(model_->loadedPath()).dir();
        const QString folderName = FileNames::getFileNameWithoutExtension(model_->document()->root()->title());
        const QString folderPath = dir.filePath(folderName);
        if (!QDir(dir.filePath(folderName)).exists() && !dir.mkdir(folderName)) {
            LOG(critical, "Could not create the dir:", folderPath)
            ErrorDialog("Could not create the folder<br><br><b>" + folderPath + "<b>").exec();
            return;
        }

        LOG(info, "Extracting to:", folderPath)
        ui_->treeWidget->selectionExtract(folderPath, model_->document()->root());
    }

    void MainWindow::selectionExtractToPrefixClick() const {
        LOG(info, "User clicked the ExtractToPrefix button")

        const QDir dir = QFileInfo(model_->loadedPath()).dir();
        const QString* unsanitizedPrefixValue = GetPrefixValueUnsanitized(*model_->document()->headers());
        assert(unsanitizedPrefixValue);
        SanitizedPath sp(*unsanitizedPrefixValue);

        const QString folderPath = dir.filePath(sp);
        if (!dir.mkpath(sp)) {
            LOG(critical, "Could not create the dir:", folderPath)
            ErrorDialog("Could not create the folder<br><br><b>" + folderPath + "<b>").exec();
            return;
        }

        LOG(info, "Extracting to:", folderPath)
        ui_->treeWidget->selectionExtract(folderPath, model_->document()->root());
    }

    void MainWindow::exportPboJsonClick() {
        LOG(info, "User clicked the ExportPboJson button - showing dialog")

        const QString configPath = QFileDialog::getSaveFileName(this, "Select the file", "pbo.json", "pbo.json;;Any file (*)");

        if (!configPath.isEmpty()) {
            LOG(info, "User selected the path:", configPath)
            model_->extractConfigurationTo(configPath);
        }
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

        if (ui_->actionSelectionExtractTo->isEnabled()) {
            LOG(debug, "actionSelectionExtract - enabled")
            menu.addAction(ui_->actionSelectionExtractTo);

            if (ui_->actionSelectionExtractFolder->isEnabled())
                menu.addAction(ui_->actionSelectionExtractFolder);

            menu.addAction(ui_->actionSelectionExtractContainer);

            if (ui_->actionSelectionExtractToPrefix->isEnabled())
                menu.addAction(ui_->actionSelectionExtractToPrefix);

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

        const PboNode* selectionRoot = ui_->treeWidget->getSelectionRoot();
        if (selectionRoot && selectionRoot->nodeType() == PboNodeType::Container) {
            LOG(debug, "container node - selected")
            menu.addSeparator();
            menu.addAction(ui_->actionExportPboJson);
        }

        if (!menu.isEmpty()) {
            LOG(debug, "Creating the context menu")
            menu.exec(ui_->treeWidget->mapToGlobal(point));
        }
        else {
            LOG(debug, "Context menu resulted empty. Not showing.")
        }

    }

    void MainWindow::treeActionStateChanged(const TreeWidget::ActionState& state) const {
        ui_->actionSelectionOpen->setEnabled(state.canOpen);
        ui_->actionSelectionRename->setEnabled(state.canRename);
        ui_->actionSelectionCopy->setEnabled(state.canCopy);
        ui_->actionSelectionCut->setEnabled(state.canCut);
        ui_->actionSelectionPaste->setEnabled(state.canPaste);
        ui_->actionSelectionDelete->setEnabled(state.canRemove);
        ui_->actionSelectionExtractTo->setEnabled(state.canExtract);

        const PboNode* selectionRoot = ui_->treeWidget->getSelectionRoot();
        if (selectionRoot) {
            ui_->actionSelectionExtractContainer->setVisible(true);
            ui_->actionSelectionExtractContainer->setEnabled(state.canExtract);
            if (selectionRoot->nodeType() == PboNodeType::Container) {
                ui_->actionSelectionExtractFolder->setEnabled(false);
                ui_->actionSelectionExtractFolder->setVisible(false);
            } else {
                ui_->actionSelectionExtractFolder->setEnabled(true);
                ui_->actionSelectionExtractFolder->setVisible(true);
                ui_->actionSelectionExtractFolder->setText(makeExtractToTitle(selectionRoot));
            }
            const QString* prefixValue = GetPrefixValueUnsanitized(*model_->document()->headers());
            if (prefixValue && !prefixValue->isEmpty()) {
                ui_->actionSelectionExtractToPrefix->setVisible(true);
                ui_->actionSelectionExtractToPrefix->setEnabled(true);
                ui_->actionSelectionExtractToPrefix->setText(makeExtractToPrefixTitle(prefixValue));
            }
        } else {
            ui_->actionSelectionExtractFolder->setEnabled(false);
            ui_->actionSelectionExtractFolder->setVisible(false);
            ui_->actionSelectionExtractContainer->setEnabled(false);
            ui_->actionSelectionExtractContainer->setVisible(false);
            ui_->actionSelectionExtractToPrefix->setVisible(false);
            ui_->actionSelectionExtractToPrefix->setEnabled(false);
        }
    }

    void MainWindow::saveFile(const QString& fileName) {
        LOG(info, "Saving the file")

        const QFuture<int> future = QtConcurrent::run([this](QPromise<int>& promise, const QString& fname) {
            model_->saveFile([&promise]() { return promise.isCanceled(); }, fname);
            promise.addResult(0);
        }, fileName);

        setIsLoading(static_cast<QFuture<void>>(future), true);

        saveWatcher_.setFuture(future);
    }

    void MainWindow::saveComplete() {
        resetIsLoading();

        QFuture<int> future = saveWatcher_.future();

        if (!future.isValid()) {
            LOG(info, "File saving was cancelled - exiting")
            return;
        }

        try {
            future.takeResult(); //to get exceptions rethrown
            LOG(info, "File saving is complete")
            setHasChanges(false);
        } catch (const DiskAccessException& ex) {
            LOG(info, "Error when saving - show error modal:", ex)
            UI_HANDLE_ERROR(ex)
        }
    }

    void MainWindow::setHasChanges(bool hasChanges) {
        LOG(info, "The HasChanges status set to:", hasChanges)
        ui_->actionFileSave->setEnabled(hasChanges);
        hasChanges_ = hasChanges;
        updateWindowTitle();
    }

    void MainWindow::updateLoadedStatus(bool loaded) const {
        LOG(info, "The Loaded status set to:", loaded)

        if (loaded) {
            ui_->treeWidget->setRoot(model_->document()->root());
            ui_->treeWidget->setDragDropMode(QAbstractItemView::DragDrop);
            ui_->actionSelectionExtractContainer->setText(makeExtractToTitle(model_->document()->root()));
            connect(model_->document()->root(), &PboNode::titleChanged, [this]() {
                ui_->actionSelectionExtractContainer->setText(makeExtractToTitle(model_->document()->root()));
            });
        } else {
            ui_->treeWidget->resetRoot();
            ui_->treeWidget->setDragDropMode(QAbstractItemView::NoDragDrop);

            ui_->actionSelectionOpen->setEnabled(false);
            ui_->actionSelectionCopy->setEnabled(false);
            ui_->actionSelectionCut->setEnabled(false);
            ui_->actionSelectionPaste->setEnabled(false);
            ui_->actionSelectionDelete->setEnabled(false);
            ui_->actionSelectionRename->setEnabled(false);
            ui_->actionSelectionExtractTo->setEnabled(false);
        }

        ui_->actionFileSaveAs->setEnabled(loaded);
        ui_->actionFileClose->setEnabled(loaded);
        ui_->actionViewHeaders->setEnabled(loaded);
        ui_->actionViewSignature->setEnabled(loaded);
        ui_->actionExportPboJson->setEnabled(loaded);
    }

    void MainWindow::updateWindowTitle() {
        if (model_->isLoaded()) {
            const QFileInfo fi(model_->loadedPath());
            const QString title = hasChanges_
                                      ? "*" + fi.fileName() + " - " + PBOM_PROJECT_NAME
                                      : fi.fileName() + " - " + PBOM_PROJECT_NAME;
            LOG(info, "Set window title to:", title)

            setWindowTitle(title);
        } else {
            LOG(info, "There is no loaded file - reset window title to the default")
            setWindowTitle(PBOM_PROJECT_NAME);
        }
    }

    void MainWindow::setIsLoading(QFuture<void> future, bool supportsCancellation) const {
        ui_->menubar->setEnabled(false);
        ui_->treeWidget->setEnabled(false);
        ui_->statusBar->progressShow(std::move(future), supportsCancellation);
    }

    void MainWindow::resetIsLoading() const {
        ui_->menubar->setEnabled(true);
        ui_->treeWidget->setEnabled(true);
        ui_->statusBar->progressHide();
    }

    QString MainWindow::makeExtractToTitle(const PboNode* node) {
        return EXTRACT_TO_TEXT + " ./" + (node->nodeType() == PboNodeType::Container
                                      ? FileNames::getFileNameWithoutExtension(node->title())
                                      : node->title())
            + (node->nodeType() == PboNodeType::File ? "" : "/");
    }

    QString MainWindow::makeExtractToPrefixTitle(const QString* prefixPath) {
        return EXTRACT_TO_TEXT + " ./" + *prefixPath + "/";
    }

    const QString MainWindow::EXTRACT_TO_TEXT = "Extract to";
}
