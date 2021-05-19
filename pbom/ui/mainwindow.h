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

    void onFileCloseClick();

    void onSelectionPasteClick();

    void onSelectionCutClick();

    QList<PboPath> onSelectionCopyClick();

    void onSelectionDeleteClick() const;

    void onModelEvent(const PboModelEvent* event);

    void treeContextMenuRequested(const QPoint& point) const;

    void treeDragStartRequested(const QList<PboPath>& paths);

    void treeDragDropped(const PboPath& target, const QMimeData* mimeData);

    void addFilesFromPbo(const PboPath& target, const QMimeData* mimeData);

    void addFilesFromFilesystem(const QList<QUrl>& urls);

    void treeSelectionChanged() const;

private:
    Ui::MainWindow* ui_;
    PboModel2* model_;
    QFutureWatcher<InteractionParcel> dragDropWatcher_;
    QFutureWatcher<InteractionParcel> cutCopyWatcher_;
    QFutureWatcher<void> saveWatcher_;
    QSharedPointer<DeleteOp> delete_;    
    BusyBar* busy_;
    bool hasChanges_;

private slots:
    void dragStartExecute() const;

    void copyOrCutExecute() const;

    void saveComplete();

    void setHasChanges(bool hasChanges);
};
