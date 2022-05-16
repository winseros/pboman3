#include "taskwindowmodel.h"
#include <QThread>
#include <QMutexLocker>
#include <QThreadPool>
#include "exception.h"
#include "util/log.h"

#define LOG(...) LOGGER("model/task/TaskWindowModel", __VA_ARGS__)

namespace pboman3::model::task {
    void TaskWindowModel::start() {
        const int numThreads = std::min(QThread::idealThreadCount(), static_cast<int>(tasks_.count()));
        for (int i = 0; i < numThreads; i++) {
            QThreadPool::globalInstance()->start(new TaskRunnable(this, i));
        }
    }

    void TaskWindowModel::stop() {
        stopped_.storeRelease(1);
    }

    void TaskWindowModel::addTask(const QSharedPointer<Task>& task) {
        tasks_.append(task);
    }

    QSharedPointer<Task> TaskWindowModel::pickNextTask() {
        QMutexLocker locker(&mutex_);

        if (tasks_.isEmpty())
            return nullptr;

        QSharedPointer<Task> task = tasks_.takeFirst();
        return task;
    }

    bool TaskWindowModel::isCancelled() const {
        return stopped_.loadAcquire() > 0;
    }

    TaskWindowModel::TaskRunnable::TaskRunnable(TaskWindowModel* model, ThreadId threadId)
        : QRunnable(),
          model_(model),
          threadId_(threadId) {
    }

    void TaskWindowModel::TaskRunnable::run() {
        emit model_->threadStarted(threadId_);

        auto cancel = [this]() {
            return model_->isCancelled();
        };

        QSharedPointer<Task> task = model_->pickNextTask();

        while (task != nullptr) {
            if (model_->isCancelled()) break;

            connect(task.get(), &Task::taskThinking, [this](const QString& text) {
                emit model_->threadThinking(threadId_, text);
            });
            connect(task.get(), &Task::taskInitialized,
                    [this](const QString& text, qint32 minProgress, qint32 maxProgress) {
                        emit model_->threadInitialized(threadId_, text, minProgress, maxProgress);
                    });
            connect(task.get(), &Task::taskProgress, [this](qint32 progress) {
                emit model_->threadProgress(threadId_, progress);
            });
            connect(task.get(), &Task::taskMessage, [this](const QString& message) {
                emit model_->threadMessage(threadId_, message);
            });

            try {
                task->execute(cancel);
            } catch (const AppException& ex) {
                LOG(warning, "Task", task, "failed with exception:", ex)
                emit model_->threadMessage(threadId_, ex.message());
            } catch (const std::exception& ex) {
                LOG(warning, "Task", task, "failed with exception:", QString(ex.what()))
                emit model_->threadMessage(threadId_, ex.what());
            }
            task = model_->pickNextTask();
        }

        emit model_->threadCompleted(threadId_);
    }
}
