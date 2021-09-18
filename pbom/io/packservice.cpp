#include "packservice.h"

#include "pboheaderreader.h"
#include "bs/pbobinarysource.h"

namespace pboman3 {

    void PackService::unpack(QPromise<void> promise, PboFile& file, const QDir& targetDir, const Cancel& cancel) const {
        PboFileHeader header = PboHeaderReader::readFileHeader(&file);
        promise.setProgressRange(0, static_cast<int>(header.entries.count()));

        size_t entryDataOffset = header.dataBlockStart;
        for (const QSharedPointer<PboEntry>& entry : header.entries) {
            if (!entry->isContent()) continue;
            const bool compressed = entry->packingMethod() == PboPackingMethod::Packed;
            PboDataInfo di{entry->originalSize(), entry->dataSize(), entryDataOffset, entry->timestamp(), compressed};
            PboBinarySource bs(file.fileName(), di);
            bs.writeToFs();
            entryDataOffset += di.dataSize;
        }
    }
}
