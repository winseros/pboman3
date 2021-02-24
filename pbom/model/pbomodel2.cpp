#include "pbomodel2.h"
#include <QDir>
#include <QUrl>
#include <QUuid>
#include "pbotreeexception.h"
#include "io/pboheaderio.h"

namespace pboman3 {
    void PboModel2::loadFile(const QString& path) {
        if (file_) {
            file_->close();
        }

        file_ = QSharedPointer<PboFile>(new PboFile(path));
        file_->open(QIODeviceBase::OpenModeFlag::ReadWrite);

        root_ = QSharedPointer<PboNode>(new PboNode("container", PboNodeType::Container, nullptr, nullptr));
        connect(root_.get(), &PboNode::onEvent, this, &PboModel2::onEvent);

        emitLoadBegin(path);

        const PboHeaderIO reader(file_.get());
        QSharedPointer<PboEntry> entry = reader.readNextEntry();

#define QUIT file_->close(); \
        const auto evt = QSharedPointer<PboLoadFailedEvent>(new PboLoadFailedEvent); \
        emit onEvent(evt.get()); \
        return;

        if (!entry) { QUIT }

        QList<QSharedPointer<PboEntry>> entries;

        if (entry->isSignature()) {
            QSharedPointer<PboHeader> header = reader.readNextHeader();
            while (header && !header->isBoundary()) {
                registerHeader(header);
                header = reader.readNextHeader();
            }
            if (!header) { QUIT }
        } else if (entry->isContent()) {
            entries.append(entry);
            root_->addEntry(entry->makePath());
        } else {
            QUIT
        }

        entry = reader.readNextEntry();
        while (entry && entry->isContent()) {
            entries.append(entry);
            root_->addEntry(entry->makePath());
            entry = reader.readNextEntry();
        }
        if (!entry || !entry->isBoundary()) { QUIT }

        size_t entryDataOffset = file_->pos();
        for (const QSharedPointer<PboEntry>& pEntry : entries) {
            PboDataInfo dataInfo{pEntry->dataSize, entryDataOffset};
            entryDataOffset += dataInfo.dataSize;
            root_->get(pEntry->makePath())->binarySource = QSharedPointer<PboBasedBinarySource>(
                new PboBasedBinarySource(path, dataInfo));
        }

        binaryBackend_ = QSharedPointer<BinaryBackend>(new BinaryBackend);

        emitLoadComplete(path);
    }

    void PboModel2::saveFile() {
        const QString tempFolderName = "pboman3";
        QDir temp(QDir::tempPath());
        if (temp.mkpath(tempFolderName)) {
            temp.cd(tempFolderName);
            PboFile tempFile(temp.filePath(QUuid::createUuid().toString(QUuid::WithoutBraces)));
            tempFile.open(QIODeviceBase::NewOnly);

            const PboHeaderIO io(&tempFile);

            writeFileSignature(io);

            for (const QSharedPointer<PboHeader>& header : headers_) {
                io.writeHeader(header.get());
            }

            PboHeader hBnd = PboHeader::makeBoundary();
            io.writeHeader(&hBnd);

            //writeFileEntries(io);

            tempFile.close();
        }
    }


    void PboModel2::createNodeSet(const PboPath& parent, const QList<QUrl>& urls) {

    }

    void PboModel2::createNodeSet(const PboPath& parent, const QByteArray& data) {

    }

    void PboModel2::moveNode(const PboPath& node, const PboPath& newParent,
                             PboConflictResolution (* onConflict)(const PboPath&, PboNodeType)) const {
        if (!root_)
            throw PboTreeException("The model is not initialized");
        root_->moveNode(node, newParent, onConflict);
    }

    void PboModel2::renameNode(const PboPath& node, const QString& title) const {
        if (!root_)
            throw PboTreeException("The model is not initialized");
        root_->renameNode(node, title);
    }

    void PboModel2::removeNode(const PboPath& node) const {
        if (!root_)
            throw PboTreeException("The model is not initialized");
        root_->removeNode(node);
    }

    InteractionData PboModel2::interactionPrepare(const QList<PboPath>& paths, const Cancel& cancel) const {
        QList<PboNode*> nodes;
        nodes.reserve(paths.length());
        for (const PboPath& p : paths) {
            QPointer<PboNode> node = root_->get(p);
            nodes.append(node);
        }

        QList<QUrl> locations = binaryBackend_->hddSync(nodes, cancel);
        QByteArray binary;
        InteractionData data{std::move(locations), std::move(binary)};
        return data;
    }

    void PboModel2::registerHeader(QSharedPointer<PboHeader>& header) {
        const PboHeaderCreatedEvent evt(header.get());
        emit onEvent(&evt);
        headers_.append(header);
    }

    void PboModel2::emitLoadBegin(const QString& path) const {
        const PboLoadBeginEvent evt(path);
        emit onEvent(&evt);
    }

    void PboModel2::emitLoadComplete(const QString& path) const {
        PboLoadCompleteEvent evt(path);
        emit onEvent(&evt);
    }

    void PboModel2::writeFileSignature(const PboHeaderIO& io) const {
        PboEntry eSig = PboEntry::makeSignature();
        io.writeEntry(&eSig);
    }
}
