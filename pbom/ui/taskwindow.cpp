#include "taskwindow.h"
#include <QFileDialog>
#include <QLayout>
#include <QPushButton>
#include "ui_taskwindow.h"
#include "model/task/taskwindowmodel.h"
#include "exception.h"
#include "win32/win32taskbarindicator.h"

namespace pboman3::ui {
    TaskWindow::TaskWindow(QWidget* parent)
        : QMainWindow(parent),
          ui_(new Ui::TaskWindow),
          activeThreadCount_(0),
          log_(nullptr),
          doneText_("Done") {
        ui_->setupUi(this);
        taskbar_ = QSharedPointer<TaskbarIndicator>(new TaskbarIndicator(winId()));
    }

    TaskWindow::~TaskWindow() {
        delete ui_;
    }

    void TaskWindow::start(const QSharedPointer<TaskWindowModel>& model) {
        if (model_)
            throw AppException("The tasks have already started");
        model_ = model;

        connect(ui_->buttonBox, &QDialogButtonBox::clicked, this, &TaskWindow::buttonClicked);

        connect(model.get(), &TaskWindowModel::threadStarted, this, &TaskWindow::threadStarted);
        connect(model.get(), &TaskWindowModel::threadThinking, this, &TaskWindow::threadThinking);
        connect(model.get(), &TaskWindowModel::threadInitialized, this, &TaskWindow::threadInitialized);
        connect(model.get(), &TaskWindowModel::threadProgress, this, &TaskWindow::threadProgress);
        connect(model.get(), &TaskWindowModel::threadCompleted, this, &TaskWindow::threadCompleted);
        connect(model.get(), &TaskWindowModel::threadMessage, this, &TaskWindow::threadMessage);

        model_->start();
    }

    void TaskWindow::threadStarted(ThreadId threadId) {
        setMaximumHeight(QWIDGETSIZE_MAX);
        activeThreadCount_++;

        const auto progress = new ProgressWidget();
        ui_->progressHost->addWidget(progress);
        progressBars_.insert(threadId, progress);

        setMaximumHeight(height());
    }

    void TaskWindow::threadThinking(ThreadId threadId, const QString& text) const {
        taskbar_->threadThinking();

        const ProgressWidget* progress = progressBars_.value(threadId);
        progress->setIndeterminate(true);
        progress->setText(text);
    }

    void TaskWindow::threadInitialized(ThreadId threadId, const QString& text, qint32 minProgress,
                                       qint32 maxProgress) const {
        taskbar_->threadInitialized(maxProgress);

        const ProgressWidget* progress = progressBars_.value(threadId);
        progress->setMinimum(minProgress);
        progress->setMaximum(maxProgress);
        progress->setValue(minProgress);
        progress->setText(text);
    }

    void TaskWindow::threadProgress(ThreadId threadId, qint32 progress) const {
        taskbar_->threadProgress(threadId, progress);

        const ProgressWidget* progressBar = progressBars_.value(threadId);
        progressBar->setValue(progress);
    }

    void TaskWindow::threadCompleted(ThreadId threadId) {
        const ProgressWidget* progress = progressBars_.value(threadId);
        progress->setIndeterminate(false);
        progress->setText(doneText_);

        activeThreadCount_--;
        if (activeThreadCount_ == 0) {
            taskbar_.clear();

            ui_->buttonBox->setEnabled(true);
            ui_->buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Close);
        }
    }

    void TaskWindow::threadMessage(ThreadId threadId, const QString& message) {
        if (!log_) {
            setMaximumHeight(QWIDGETSIZE_MAX);
            log_ = new QPlainTextEdit(this);
            log_->setReadOnly(true);
            log_->setMinimumHeight(100);
            log_->setLineWrapMode(QPlainTextEdit::NoWrap);
            log_->setCenterOnScroll(true);
            ui_->progressHost->addWidget(log_);
        }
        log_->appendPlainText(message);
    }

    void TaskWindow::buttonClicked(const QAbstractButton* button) {
        if (button == dynamic_cast<QAbstractButton*>(ui_->buttonBox->button(QDialogButtonBox::Cancel))) {
            ui_->buttonBox->setEnabled(false);
            doneText_ = "Cancelled";
            model_->stop();
        } else if (button == dynamic_cast<QAbstractButton*>(ui_->buttonBox->button(QDialogButtonBox::Close))) {
            close();
        }
    }

    TaskWindow::TaskbarIndicator::TaskbarIndicator(WId windowId)
        : maxValue_(0),
          currentValue_(0) {
        taskbar_ = QSharedPointer<ui::TaskbarIndicator>(new Win32TaskbarIndicator(windowId));
    }

    void TaskWindow::TaskbarIndicator::threadThinking() const {
        if (currentValue_ == 0)
            taskbar_->setIndeterminate();
    }

    void TaskWindow::TaskbarIndicator::threadInitialized(qint64 maxProgress) {
        maxValue_ += maxProgress;
        taskbar_->setProgressValue(currentValue_, maxValue_);
    }

    void TaskWindow::TaskbarIndicator::threadProgress(ThreadId threadId, qint32 progress) {
        const qint32 threadValue = threadValues_.value(threadId, 0);
        const qint32 increment = progress - threadValue;
        threadValues_.insert(threadId, progress);
        currentValue_ += increment;
        taskbar_->setProgressValue(currentValue_, maxValue_);
    }
}
