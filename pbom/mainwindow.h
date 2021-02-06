#pragma once

#include <QMainWindow>
#include <QSharedPointer>
#include <QAction>
#include "treemodel.h"
#include "model/pbomodelevents.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
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

    void onSelectionDeleteClick() const;

    void onModelEvent(const PboModelEvent* event);

    void onContextMenuRequested(const QPoint& point);

private:
    Ui::MainWindow* ui_;
    QSharedPointer<TreeModel> treeModel_;

    void onTreeNodeExpanded(const QModelIndex& index) const;

    void onTreeNodeCollapsed(const QModelIndex& index) const;

    void onLoadBegin(const PboLoadBeginEvent* event);

    void onLoadComplete(const PboLoadCompleteEvent* event);

    void onLoadFailed(const PboLoadFailedEvent* event);

    void onHeaderCreated(const PboHeaderCreatedEvent* event);

    void onEntryUpdated(const PboEntryCreatedEvent* event);
};
