#pragma once

#include <QString>

namespace pboman3 {
    class SanitizedString {
    public:
        SanitizedString(const QString& text);

        operator const QString&();

    private:
        const QString* originalText_;
        QString sanitizedText_;

        static void replaceAll(QString& text, QChar what, const QString& with);

        static void replaceEnd(QString& text, QChar what, const QString& with);
    };
}
