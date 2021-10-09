#include "rootreader.h"
#include "io/bs/pbobinarysource.h"
#include "util/log.h"

#define LOG(...) LOGGER("model/RootReader", __VA_ARGS__)

namespace pboman3 {
    RootReader::RootReader(const PboFileHeader* header, const QString& path)
        : header_(header),
          path_(path) {
    }

    void RootReader::inflateRoot(PboNode* root) const {
        LOG(info, "Inflating the nodes hierarchy")
        qsizetype entryDataOffset = header_->dataBlockStart;
        for (const QSharedPointer<PboEntry>& entry : header_->entries) {
            LOG(debug, "Processing the entry:", *entry)
            PboNode* node = root->createHierarchy(entry->makePath());
            PboDataInfo dataInfo{0, 0, 0, 0, 0};
            dataInfo.originalSize = entry->originalSize();
            dataInfo.dataSize = entry->dataSize();
            dataInfo.dataOffset = entryDataOffset;
            dataInfo.timestamp = entry->timestamp();
            dataInfo.compressed = entry->packingMethod() == PboPackingMethod::Packed;
            entryDataOffset += dataInfo.dataSize;
            node->binarySource = QSharedPointer<PboBinarySource>(
                new PboBinarySource(path_, dataInfo));
            node->binarySource->open();
        }

    }
}
