#pragma once

#include <QAction>
#include <QDropEvent>
#include <QMainWindow>
#include "busybar.h"
#include "model/pbomodel.h"
#include "treewidget/treewidget.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

namespace pboman3 {
    class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent, PboModel* model);

        ~MainWindow();

        void loadFile(const QString& fileName);

    protected:
        void closeEvent(QCloseEvent* event) override;

    private:
        Ui::MainWindow* ui_;
        PboModel* model_;
        QFutureWatcher<int> saveWatcher_;
        BusyBar* busy_;
        bool hasChanges_;

        void unloadFile();

        void setupConnections();

        void onFileOpenClick();

        void onFileSaveClick();

        void onFileSaveAsClick();

        void onFileCloseClick();

        void onViewHeadersClick();

        void onViewSignatureClick();

        bool queryCloseUnsaved();

        void treeContextMenuRequested(const QPoint& point) const;

        void treeActionStateChanged(const TreeWidget::ActionState& state) const;

        void saveFile(const QString& fileName);

        void saveComplete();

        void setHasChanges(bool hasChanges);

        void setLoaded(bool loaded) const;

        void updateWindowTitle();
    };
}
