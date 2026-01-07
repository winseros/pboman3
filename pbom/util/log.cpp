#include "log.h"
#include <QLoggingCategory>

namespace pboman3::util {
    LogWorker::LogWorker(QtMessageHandler implementation)
        : implementation_(implementation) {
    }

    void LogWorker::handleMessage(QtMsgType type, const QString& file, const QString& message) const {
        implementation_(type, QMessageLogContext(file.toUtf8(), 0, nullptr, nullptr), message);
    }

    LoggingInfrastructure::LoggingInfrastructure()
        : worker_(nullptr),
          defaultHandler_(nullptr) {
        assert(!LoggingInfrastructure::logging_);
        logging_ = this;
    }

    LoggingInfrastructure::~LoggingInfrastructure() {
        assert(LoggingInfrastructure::logging_ == this);
        logging_ = nullptr;

        thread_.exit();
        thread_.wait();

        delete worker_;

        if (defaultHandler_)
            qInstallMessageHandler(defaultHandler_);
    }

    void LoggingInfrastructure::run(const ApplicationLogLevel logLevel) {
        assert(!defaultHandler_ && !worker_);

        SetLoggerParameters(logLevel);

        defaultHandler_ = qInstallMessageHandler(handleMessage);
        worker_ = new LogWorker(defaultHandler_);
        worker_->moveToThread(&thread_);
        thread_.start(QThread::LowPriority);
        connect(this, &LoggingInfrastructure::messageReceived, worker_, &LogWorker::handleMessage);
    }

    void LoggingInfrastructure::handleMessage(QtMsgType type,
                                              const QMessageLogContext& context,
                                              const QString& message) {
        assert(LoggingInfrastructure::logging_);
        emit logging_->messageReceived(type, context.file, message);
    }

    void SetLoggerParameters(const ApplicationLogLevel logLevel) {
        qSetMessagePattern(
            "%{time yyyy-MM-dd HH:mm:ss.zzz}|%{if-debug}DBG%{endif}%{if-info}INF%{endif}%{if-warning}WRN%{endif}%{if-critical}CRT%{endif}%{if-fatal}FTL%{endif}|%{file}|%{message}");
#ifdef NDEBUG
        switch (logLevel) {
            case ApplicationLogLevel::INFO:
                QLoggingCategory::setFilterRules("*.debug=false");
                break;
            case ApplicationLogLevel::WARN:
                QLoggingCategory::setFilterRules("*.debug=false\n*.info=false");
                break;
            default:
                break;
        }

#endif
    }

    LoggingInfrastructure* LoggingInfrastructure::logging_ = nullptr;
}
