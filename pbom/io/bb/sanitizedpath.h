#pragma once

#include <QList>

namespace pboman3::io {
    class SanitizedPath {
    public:
        explicit SanitizedPath(const QString& path);

        operator const QString&();

    private:
        static QString segmentsJoin(const QList<QString>& segments);

        static QList<QString> getSegmentsSanitized(const QList<QString>& segments);

        QString sanitizedText_;
    };
}