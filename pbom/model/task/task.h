#pragma once

#include <QObject>
#include "util/util.h"

namespace pboman3::model {
    class Task : public QObject {
    Q_OBJECT

    public:
        virtual void execute(const Cancel& cancel) = 0;

    signals:
        void taskThinking(const QString& text);

        void taskInitialized(const QString& text, qint32 minProgress, qint32 maxProgress);

        void taskProgress(qint32 progress);

        void taskMessage(const QString& message);
    };
}
