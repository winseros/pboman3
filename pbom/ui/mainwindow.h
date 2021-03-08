#pragma once

#include <QAction>
#include <QDropEvent>
#include <QFutureWatcher>
#include <QMainWindow>
#include <QProgressBar>
#include "model/pbomodel2.h"
#include "model/pbomodelevents.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

using namespace pboman3;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow();

public slots:
    void onFileOpenClick();

    void onFileSaveClick();

    void onSelectionPasteClick();

    void onSelectionCutClick();

    QList<PboPath> onSelectionCopyClick();

    void onSelectionDeleteClick() const;

    void onModelEvent(const PboModelEvent* event) const;

    void treeContextMenuRequested(const QPoint& point) const;

    void treeDragStartRequested(const QList<PboPath>& paths);

    void treeDragDropped(const PboPath& target, const QMimeData* mimeData);

    void treeSelectionChanged() const;

private:
    Ui::MainWindow* ui_;
    PboModel2 model_;
    QFutureWatcher<InteractionData> dragDropWatcher_;
    QFutureWatcher<InteractionData> cutCopyWatcher_;
    QList<PboPath> pendingCutOp_;
    QProgressBar* busy_;
    int busyCount_;

    void setBusy();

    void resetBusy();

private slots:
    void dragStartExecute();

    void copyOrCutExecute();
};
