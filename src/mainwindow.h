#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

    void onModelEvent(const PboModelEvent* event);

private:
    Ui::MainWindow* ui;

    void onLoadBegin(const PboLoadBeginEvent* event);

    void onLoadComplete(const PboLoadCompleteEvent* event);

    void onLoadFailed(const PboLoadFailedEvent* event);

    void onHeaderUpdated(const PboHeaderUpdatedEvent* event);

    void onEntryUpdated(const PboEntryUpdatedEvent* event);
};

#endif // MAINWINDOW_H
