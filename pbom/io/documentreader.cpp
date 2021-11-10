#include "documentreader.h"
#include <QFileInfo>
#include "pboheaderreader.h"
#include "bs/pbobinarysource.h"

namespace pboman3::io {
    DocumentReader::DocumentReader(QString path)
        : path_(std::move(path)) {
    }

    QSharedPointer<PboDocument> DocumentReader::read() const {
        PboFile pbo(path_);
        pbo.open(QIODeviceBase::ReadOnly);
        PboFileHeader header = PboHeaderReader::readFileHeader(&pbo);

        const QFileInfo fi(path_);

        QSharedPointer<PboDocument> document(new PboDocument(fi.fileName()));

        for (const QSharedPointer<PboHeader>& h : header.headers) {
            document->headers()->add(h->name, h->value);
        }

        qsizetype entryDataOffset = header.dataBlockStart;
        for (const QSharedPointer<PboEntry>& e : header.entries) {
            DocumentNode* node = document->root()->createHierarchy(e->makePath());
            PboDataInfo dataInfo{0, 0, 0, 0, 0};
            dataInfo.originalSize = e->originalSize();
            dataInfo.dataSize = e->dataSize();
            dataInfo.dataOffset = entryDataOffset;
            dataInfo.timestamp = e->timestamp();
            dataInfo.compressed = e->packingMethod() == PboPackingMethod::Packed;
            entryDataOffset += dataInfo.dataSize;
            node->binarySource = QSharedPointer<PboBinarySource>(
                new PboBinarySource(path_, dataInfo));
            node->binarySource->open();
        }

        document->setSignature(std::move(header.signature));

        return document;
    }
}
