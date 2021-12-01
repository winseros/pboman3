#include "documentreader.h"
#include <QFileInfo>
#include "diskaccessexception.h"
#include "pboheaderreader.h"
#include "bs/pbobinarysource.h"

namespace pboman3::io {
    DocumentReader::DocumentReader(QString path)
        : path_(std::move(path)) {
    }

    QSharedPointer<PboDocument> DocumentReader::read() const {
        PboFile pbo(path_);
        if (!pbo.open(QIODeviceBase::ReadOnly)) {
            throw DiskAccessException("Can not access the file. Check if it is used by other processes.", path_);
        }
        PboFileHeader header = PboHeaderReader::readFileHeader(&pbo);

        QList<QSharedPointer<DocumentHeader>> headers;
        headers.reserve(header.headers.count());
        for (const QSharedPointer<PboHeaderEntity>& h : header.headers)
            headers.append(QSharedPointer<DocumentHeader>(new DocumentHeader(DocumentHeader::InternalData{ h->name, h->value })));

        const QFileInfo fi(path_);
        QSharedPointer<PboDocument> document(new PboDocument(fi.fileName(), std::move(headers), std::move(header.signature)));

        qsizetype entryDataOffset = header.dataBlockStart;
        for (const QSharedPointer<PboNodeEntity>& e : header.entries) {
            PboNode* node = document->root()->createHierarchy(e->makePath());
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

        return document;
    }
}
