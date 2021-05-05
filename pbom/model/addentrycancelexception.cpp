#include "addentrycancelexception.h"

namespace pboman3 {

    AddEntryCancelException::AddEntryCancelException(QString message)
        : AppException(std::move(message)) {
    }

    QDebug operator<<(QDebug debug, const AddEntryCancelException& ex) {
        return debug << "AddEntryCancelException: " << ex.message();
    }

    void AddEntryCancelException::raise() const {
        throw*this;
    }

    QException* AddEntryCancelException::clone() const {
        return new AddEntryCancelException(*this);
    }
}
