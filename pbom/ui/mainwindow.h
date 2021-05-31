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

using namespace pboman3;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget* parent, PboModel* model);

    ~MainWindow();

    void setupConnections();

    void onFileOpenClick();

    void onFileSaveClick();

    void onFileCloseClick();

    void treeContextMenuRequested(const QPoint& point) const;

    void treeActionStateChanged(const TreeWidget::ActionState& state) const;

private:
    Ui::MainWindow* ui_;
    PboModel* model_;
    QFutureWatcher<void> saveWatcher_;
    BusyBar* busy_;
    bool hasChanges_;

    void saveComplete();

    void setHasChanges(bool hasChanges);

    void setLoaded(bool loaded) const;
};
