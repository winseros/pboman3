#pragma once

#include "documentreader.h"

namespace pboman3::io {
    DocumentReader CreateDocumentReader(QString path, bool junkFilterEnable);
}
