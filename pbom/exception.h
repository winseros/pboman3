#pragma once

#include <QException>

namespace pboman3 {
    class AppException : public QException {
    public:
        AppException(QString message);

        const QString& message() const;

        friend QDebug& operator<<(QDebug& debug, const AppException& ex);

        virtual QDebug& dump(QDebug& debug) const;

        void raise() const override;

        QException* clone() const override;

    protected:
        const QString message_;
    };


#define PBOMAN_EX_HEADER(ExType) \
    friend QDebug operator<<(QDebug debug, const ExType& ex); \
    void raise() const override; \
    QException* clone() const override; \
    QDebug& dump(QDebug& debug) const override;

#define PBOMAN_EX_IMPL(ExType) \
    QDebug operator<<(QDebug debug, const ExType& ex) { \
        return ex.dump(debug); \
    } \
    void ExType::raise() const { \
        throw* this;\
    }\
    QException* ExType::clone() const {\
        return new ExType(*this);\
    }

#define PBOMAN_EX_IMPL_DEFAULT(ExType) PBOMAN_EX_IMPL(ExType) \
    QDebug& ExType::dump(QDebug& debug) const { \
        return debug << #ExType << "(" << message_ << ")"; \
    }

    class InvalidOperationException : public AppException {
    public:
        InvalidOperationException(QString message);

        PBOMAN_EX_HEADER(InvalidOperationException)
    };
}
