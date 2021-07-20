#include "stringbuilder.h"

namespace pboman3 {
    StringBuilder::StringBuilder(size_t bufSize)
        : value(new TCHAR[bufSize]),
          bufSize_(bufSize),
          dataSize_(0) {
    }

    StringBuilder::~StringBuilder() {
        delete[] value;
    }

    StringBuilder& StringBuilder::append(LPCTSTR str) {
        const size_t size = strlen(str);
        strcpy_s(value + dataSize_, bufSize_ - dataSize_, str);
        dataSize_ += size;
        return *this;
    }
}
