#pragma once

#include <QDebug>

#define M_NUM_ARGS_10TH(A10, A9, A8, A7, A6, A5, A4, A3, A2, A1, A0, ...) A0
#define M_NUM_ARGS(...) M_NUM_ARGS_10TH(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define M_LOG_IMPL_A1(A1) A1
#define M_LOG_IMPL_A2(A1, A2) M_LOG_IMPL_A1(A1) << A2
#define M_LOG_IMPL_A3(A1, A2, A3) M_LOG_IMPL_A2(A1, A2) << A3
#define M_LOG_IMPL_A4(A1, A2, A3, A4) M_LOG_IMPL_A3(A1, A2, A3) << A4
#define M_LOG_IMPL_A5(A1, A2, A3, A4, A5) M_LOG_IMPL_A4(A1, A2, A3, A4) << A5
#define M_LOG_IMPL_A6(A1, A2, A3, A4, A5, A6) M_LOG_IMPL_A5(A1, A2, A3, A4, A5) << A6
#define M_LOG_IMPL_A7(A1, A2, A3, A4, A5, A6, A7) M_LOG_IMPL_A6(A1, A2, A3, A4, A5, A6) << A7
#define M_LOG_IMPL_A8(A1, A2, A3, A4, A5, A6, A7, A8) M_LOG_IMPL_A7(A1, A2, A3, A4, A5, A6, A7) << A8
#define M_LOG_IMPL_A9(A1, A2, A3, A4, A5, A6, A7, A8, A9) M_LOG_IMPL_A8(A1, A2, A3, A4, A5, A6, A7, A8) << A9
#define M_LOG_IMPL_A10(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) M_LOG_IMPL_A9(A1, A2, A3, A4, A5, A6, A7, A8, A9) << A10

#define M_LOG_IMPL3(TARGET, N, ...) TARGET << M_LOG_IMPL_A##N(__VA_ARGS__)
#define M_LOG_IMPL2(TARGET, N, ...) M_LOG_IMPL3(TARGET, N, __VA_ARGS__)
#define M_LOG_IMPL(LOG, LEVEL, ...) M_LOG_IMPL2(LOG.LEVEL(), M_NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

#define LOGGER(CLASS, ...) M_LOG_IMPL(QMessageLogger(CLASS, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC), __VA_ARGS__);

/*
USE THIS CODE FOR MACRO DEBUGGING
#define __DEBUG_STRING2(x) #x
#define DEBUG_STRING(x) __DEBUG_STRING2(x)
#pragma message(DEBUG_STRING(LOGGER("Main", debug, "Hello!")))
*/

#include <QScopedPointer>
#include <QThread>

#define ACTIVATE_ASYNC_LOG_SINK auto logging = QScopedPointer(new util::LoggingInfrastructure); logging->run();

namespace pboman3::util {
    /*These two guys make standard QT logs be output on a non UI-thread*/
    /*as UI-thread output has too big UI responsiveness penalty*/
    class LogWorker : public QObject {
    Q_OBJECT
    public:
        LogWorker(QtMessageHandler implementation);

    public slots:
        void handleMessage(QtMsgType type, const QString& file, const QString& message) const;

    private:
        QtMessageHandler implementation_;
    };

    class LoggingInfrastructure : public QObject {
    Q_OBJECT
    public:
        LoggingInfrastructure();

        ~LoggingInfrastructure() override;

        void run();

    signals:
        void messageReceived(QtMsgType type, const QString& file, const QString& message);

    private:
        static LoggingInfrastructure* logging_;

        static void handleMessage(QtMsgType type, const QMessageLogContext& context, const QString& message);

        QThread thread_;
        LogWorker* worker_;
    };
}
