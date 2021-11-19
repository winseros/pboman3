#pragma once

#include <QFuture>
#include <QProgressBar>
#include <QPushButton>
#include <QStatusBar>
#include <QWidget>

namespace pboman3::ui {
    class StatusBar : public QStatusBar {
    Q_OBJECT
    public:
        StatusBar(QWidget* parent = nullptr);

    public slots:
        void progressShow(QFuture<void> future);

        void progressHide() const;

    signals:
        void cancelRequested();

    private:
        QFuture<void> future_;
        QProgressBar* progress_;
        QPushButton* button_;
    };
}
