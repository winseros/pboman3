#include "sanitizedstring.h"

namespace pboman3::io {
    SanitizedString::SanitizedString(const QString& text)
        : originalText_(nullptr) {

        QString sanitized = text; //https://doc.qt.io/qt-6/implicit-sharing.html
		replaceAll(sanitized, '\t', "%09");
		replaceAll(sanitized, '?', "%3F");
		replaceAll(sanitized, '*', "%2A");
		replaceEnd(sanitized, '.', "%2E");

        if (sanitized.isEmpty())
            originalText_ = &text;
        else
            sanitizedText_ = sanitized;
    }

    void SanitizedString::replaceAll(QString& text, QChar what, const QString& with) {
        if (text.contains(what)) {
            text.replace(what, with);
        }
    }

    void SanitizedString::replaceEnd(QString& text, QChar what, const QString& with) {
		if (text.endsWith(what)) {
			text.replace(text.count() - 1, 1, with);
		}
    }

    SanitizedString::operator const QString&() {
        return sanitizedText_.isEmpty() ? *originalText_ : sanitizedText_;
    }
}
