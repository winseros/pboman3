#include "sanitizedpath.h"
#include "sanitizedstring.h"
#include "domain/pbopath.h"
#include <QString>
#include <QDir>

namespace pboman3::io {
    using namespace domain;

    SanitizedPath::SanitizedPath(const QString& path)  {
        if (path.isEmpty())
            return;

        const PboPath segments(path);
        if (segments.empty())
            return;

        const QList<QString> segmentsSanitized = getSegmentsSanitized(segments);
        sanitizedText_ = segmentsJoin(segmentsSanitized);
    }

    SanitizedPath::operator const QString&() {
        return sanitizedText_;
    }

    QString SanitizedPath::segmentsJoin(const QList<QString>& segments) {
        QString result;

        auto it = segments.begin();
        while (it != segments.end()) {
            if (!result.isEmpty())
                result.append(QDir::separator());

            result.append(*it);
            ++it;
        }

        return result;
    }

    QList<QString> SanitizedPath::getSegmentsSanitized(const QList<QString>& segments) {
        QList<QString> sanitized;
        sanitized.reserve(segments.length());

        auto it = segments.begin();
        while (it != segments.end()) {
            sanitized.append(SanitizedString(*it));
            ++it;
        }

        return sanitized;
    }
}
