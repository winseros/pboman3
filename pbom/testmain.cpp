#include <gtest/gtest.h>
#include <iostream>
#include <QTime>

void TestLogger(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    const QByteArray localMsg = msg.toLocal8Bit();
    const char* file = context.file ? context.file : "";
    const char* function = context.function ? context.function : "";
    const QByteArray timeStr = QTime::currentTime().toString(Qt::ISODateWithMs).toLocal8Bit();

    switch (type) {
    case QtDebugMsg:
        std::cout << timeStr.constData() << "|DBG|" << localMsg.constData() << "|file: " << file << ", line: " << context.line << ", func: " << function << std::endl;
        break;
    case QtInfoMsg:
        std::cout << timeStr.constData() << "|INF|" << localMsg.constData() << "|" << file << ", line " << context.line << "|" << function << std::endl;
        break;
    case QtWarningMsg:
        std::cout << timeStr.constData() << "|WRN|" << localMsg.constData() << "|" << file << ", line " << context.line << "|" << function << std::endl;
        break;
    case QtCriticalMsg:
        std::cout << timeStr.constData() << "|CRT|" << localMsg.constData() << "|" << file << ", line " << context.line << "|" << function << std::endl;
        break;
    case QtFatalMsg:
        std::cout << timeStr.constData() << "|FTL|" << localMsg.constData() << "|" << file << ", line " << context.line << "|" << function << std::endl;
        break;
    }
}

int main(int argc, char* argv[]) {
    qInstallMessageHandler(TestLogger);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
