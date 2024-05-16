#pragma once

#include <QString>
#include <QList>

namespace pboman3::io {
    class SanitizedString {
    public:
        explicit SanitizedString(const QString& text);

        operator const QString&();

    private:
        static const QList<QString> restrictedFileNames_;

        QString sanitizedText_;

        static bool needsCharacterSanitization(const QString& text, qsizetype* firstInvalidCharIndex);

        static QString doCharacterSanitization(const QString& text, qsizetype firstInvalidCharIndex);

        static bool needsKeywordSanitization(const QString& text, QString* keyword);

        static QString doKeywordSanitization(const QString& text, const QString& keyword);

        static bool isCharLegal(const QChar& chr);

        static QString sanitizeChar(const QChar& chr);
    };
}
