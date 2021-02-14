#include "pbomodel2.h"
#include <QDir>
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
        QSharedPointer<PboEntry_> entry = reader.readNextEntry();

#define QUIT file_->close(); \
        const auto evt = QSharedPointer<PboLoadFailedEvent>(new PboLoadFailedEvent); \
        emit onEvent(evt.get()); \
        return;

        if (!entry) { QUIT }

        if (entry->isSignature()) {
            QSharedPointer<PboHeader> header = reader.readNextHeader();
            while (header && !header->isBoundary()) {
                registerHeader(header);
                header = reader.readNextHeader();
            }
            if (!header) { QUIT }
        } else if (entry->isContent()) {
            root_->addEntry(entry.get());
        } else {
            QUIT
        }

        entry = reader.readNextEntry();
        while (entry && entry->isContent()) {
            root_->addEntry(entry.get());
            entry = reader.readNextEntry();
        }
        if (!entry || !entry->isBoundary()) { QUIT }

        //updateEntriesOffsets(static_cast<long>(file_->pos()));
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
