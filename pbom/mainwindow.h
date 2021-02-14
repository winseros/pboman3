#pragma once

#include <QAction>
#include <QMainWindow>
#include "treemodel.h"
#include "model/pbomodel2.h"
#include "model/pbomodelevents.h"

#include "ui/treecontrol.h"

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
    PboModel2 model_;
    QSharedPointer<TreeControl> treeCtrl_;
};
