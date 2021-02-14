#include "pbotreeexception.h"

namespace pboman3 {

    PboTreeException::PboTreeException(QString message)
        : QException(),
          message_(std::move(message)) {
    }

    const QString& PboTreeException::message() const {
        return message_;
    }
}
