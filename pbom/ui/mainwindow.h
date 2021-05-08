#pragma once

#include <QAction>
#include <QDropEvent>
#include <QFutureWatcher>
#include <QMainWindow>
#include "busybar.h"
#include "deleteop.h"
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
    explicit MainWindow(QWidget* parent, PboModel2* model);

    ~MainWindow();

public slots:
    void onFileOpenClick();

    void onFileSaveClick();

    void onSelectionPasteClick() const;

    void onSelectionCutClick();

    QList<PboPath> onSelectionCopyClick();

    void onSelectionDeleteClick() const;

    void appendFilesToModel(const QList<QUrl>& urls) const;

    void onModelEvent(const PboModelEvent* event) const;

    void treeContextMenuRequested(const QPoint& point) const;

    void treeDragStartRequested(const QList<PboPath>& paths);

    void treeDragDropped(const PboPath& target, const QMimeData* mimeData);

    void treeSelectionChanged() const;

private:
    Ui::MainWindow* ui_;
    PboModel2* model_;
    QFutureWatcher<InteractionParcel> dragDropWatcher_;
    QFutureWatcher<InteractionParcel> cutCopyWatcher_;
    QSharedPointer<DeleteOp> delete_;    
    BusyBar* busy_;

private slots:
    void dragStartExecute() const;

    void copyOrCutExecute() const;
};
