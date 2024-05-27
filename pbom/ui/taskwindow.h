#pragma once

#include <QAbstractButton>
#include <QMainWindow>
#include <QPlainTextEdit>
#include "taskbarindicator.h"
#include "model/task/taskwindowmodel.h"
#include "progresswidget/progresswidget.h"

namespace Ui {
    class TaskWindow;
}

namespace pboman3::ui {
    using namespace model::task;

    class TaskWindow : public QMainWindow {
    Q_OBJECT

    public:
        TaskWindow(QWidget* parent, TaskWindowModel* model);

        ~TaskWindow() override;

        void showAndRunTasks();

    private:
        class TaskbarIndicator;

        Ui::TaskWindow* ui_;
        TaskWindowModel* model_;
        int activeThreadCount_;
        QHash<ThreadId, ProgressWidget*> progressBars_;
        QPlainTextEdit* log_;
        QString doneText_;
//        QSharedPointer<TaskbarIndicator> taskbar_;

        void setupConnections();

        void threadStarted(ThreadId threadId);

        void threadThinking(ThreadId threadId, const QString& text) const;

        void threadInitialized(ThreadId threadId, const QString& text, qint32 minProgress, qint32 maxProgress) const;

        void threadProgress(ThreadId threadId, qint32 progress) const;

        void threadCompleted(ThreadId threadId);

        void threadMessage(ThreadId threadId, const QString& message);

        void buttonClicked(const QAbstractButton* button);

        bool anyMessagesInTheLog() const;

        void closeWindowIfNeeded();

        class TaskbarIndicator {
        public:
            TaskbarIndicator(WId windowId);

            void threadThinking() const;

            void threadInitialized(qint64 maxProgress);

            void threadProgress(ThreadId threadId, qint32 progress);

        private:
            qint64 maxValue_;
            qint64 currentValue_;
            QHash<ThreadId, qint32> threadValues_;
            QSharedPointer<ui::TaskbarIndicator> taskbar_;
        };
    };
}
