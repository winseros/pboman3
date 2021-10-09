#pragma once

#include <QAbstractButton>
#include <QMainWindow>
#include <QPlainTextEdit>
#include "model/task/taskwindowmodel.h"
#include "progresswidget/progresswidget.h"

namespace Ui {
    class TaskWindow;
}

namespace pboman3 {
    class TaskWindow : public QMainWindow {
    Q_OBJECT

    public:
        TaskWindow(QWidget* parent);

        ~TaskWindow() override;

    protected:
        void start(const QSharedPointer<TaskWindowModel>& model);

    private:
        Ui::TaskWindow* ui_;
        QSharedPointer<TaskWindowModel> model_;
        int activeThreadCount_;
        QHash<ThreadId, ProgressWidget*> progressBars_;
        QPlainTextEdit* log_;
        QString doneText_;

        void threadStarted(ThreadId threadId);

        void threadThinking(ThreadId threadId, const QString& text) const;

        void threadInitialized(ThreadId threadId, const QString& text, qint32 minProgress, qint32 maxProgress) const;

        void threadProgress(ThreadId threadId, qint32 progress) const;

        void threadCompleted(ThreadId threadId);

        void threadMessage(ThreadId threadId, const QString& message);

        void buttonClicked(QAbstractButton* button);
    };
}
