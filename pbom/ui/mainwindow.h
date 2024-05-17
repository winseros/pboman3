#pragma once

#include <QAction>
#include <QDropEvent>
#include <QMainWindow>
#include "model/pbomodel.h"
#include "treewidget/treewidget.h"

namespace Ui {
    class MainWindow;
}

namespace pboman3::ui {
    using namespace model;

    class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent, model::PboModel* model);

        ~MainWindow() override;

        void loadFile(const QString& fileName);

    protected:
        void closeEvent(QCloseEvent* event) override;

    private:
        Ui::MainWindow* ui_;
        PboModel* model_;
        QFutureWatcher<int> saveWatcher_;
        QFutureWatcher<int> loadWatcher_;
        bool hasChanges_;

        void loadComplete();

        void unloadFile();

        void setupConnections();

        void onFileOpenClick();

        void onFileSaveClick();

        void onFileSaveAsClick();

        void onFileCloseClick();

        void onViewHeadersClick();

        void onViewSignatureClick();

        void selectionExtractToClick();

        void selectionExtractFolderClick() const;

        void selectionExtractContainerClick() const;

        void exportPboJsonClick();

        bool queryCloseUnsaved();

        void treeContextMenuRequested(const QPoint& point) const;

        void treeActionStateChanged(const TreeWidget::ActionState& state) const;

        void saveFile(const QString& fileName);

        void saveComplete();

        void setHasChanges(bool hasChanges);

        void updateLoadedStatus(bool loaded) const;

        void updateWindowTitle();

        void setIsLoading(QFuture<void> future, bool supportsCancellation) const;

        void resetIsLoading() const;

        static QString makeExtractToTitle(const PboNode* node) ;
    };
}
