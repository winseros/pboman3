#pragma once

#include <QAtomicInt>
#include <QMutex>
#include <QRunnable>
#include <QSharedPointer>
#include "task.h"

namespace pboman3::model::task {
    typedef qint32 ThreadId;

    class TaskWindowModel : public QObject {
    Q_OBJECT

    public:
        void start();

        void stop();

    signals:
        void threadStarted(ThreadId threadId);

        void threadThinnking(ThreadId threadId, const QString& text);

        void threadInitialized(ThreadId threadId, const QString& text, qint32 progressMin, qint32 progressMax);

        void threadProgress(ThreadId threadId, qint32 progress);

        void threadCompleted(ThreadId threadId);

        void threadMessage(ThreadId threadId, const QString& message);

    protected:
        void addTask(const QSharedPointer<Task>& task);

    private:
        QMutex mutex_;
        QList<QSharedPointer<Task>> tasks_;
        QAtomicInt stopped_;

        QSharedPointer<Task> pickNextTask();

        bool isCancelled() const;

        class TaskRunnable : public QRunnable {
        public:
            TaskRunnable(TaskWindowModel* model, ThreadId threadId);

            void run() override;

        private:
            TaskWindowModel* model_;
            ThreadId threadId_;
        };
    };
};
