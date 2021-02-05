#include "pbomodel.h"
#include <QDir>
#include <QUuid>
#include <memory>
#include "io/pboheaderio.h"

namespace pboman3 {
    using namespace std;

    PboModel* PboModel::instance = new PboModel;

    void PboModel::loadFile(const QString& path) {
        if (file_) {
            file_->close();
        }

        file_ = QSharedPointer<PboFile>(new PboFile(path));
        file_->open(QIODeviceBase::OpenModeFlag::ReadWrite);
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
            registerEntry(entry);
        } else {
            QUIT
        }

        entry = reader.readNextEntry();
        while (entry && entry->isContent()) {
            registerEntry(entry);
            entry = reader.readNextEntry();
        }
        if (!entry || !entry->isBoundary()) { QUIT }

        updateEntriesOffsets(static_cast<long>(file_->pos()));
        emitLoadComplete(path);
    }

    void PboModel::saveFile() {
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

            writeFileEntries(io);

            tempFile.close();
        }
    }

    void PboModel::createEntry(const QString& systemFilePath, const QString& pboFilePath) {
        // additions_.push_back(QSharedPointer<ChangeAdd>(new ChangeAdd(systemFilePath, pboFilePath)));
    }

    void PboModel::moveEntry(const PboEntry* entry, const QString& pboFilePath) {
        // const auto* ent = dynamic_cast<const PboEntry_*>(entry);
        // assert(ent && "Must have a special type");
        // transforms_.push_back(QSharedPointer<ChangeMove>(new ChangeMove(ent, pboFilePath)));
    }

    void PboModel::scheduleEntryDelete(const PboEntry* entry) {
        if (!pendingDeletes_.contains(entry->fileName)) {
            pendingDeletes_.insert(entry->fileName);
            const QSharedPointer<PboEntryDeleteScheduledEvent> evt(new PboEntryDeleteScheduledEvent(entry));
            emit onEvent(evt.get());
        }
    }

    void PboModel::cancelEntryDelete(const PboEntry* entry) {
        if (pendingDeletes_.remove(entry->fileName)) {
            const QSharedPointer<PboEntryDeleteScheduledEvent> evt(new PboEntryDeleteScheduledEvent(entry));
            emit onEvent(evt.get());
        }
    }

    void PboModel::updateEntriesOffsets(long offsetStart) {
        for (QSharedPointer<PboEntry_>& entry : entries_) {
            entry->dataOffset = offsetStart;
            offsetStart += entry->dataSize;
        }
    }

    void PboModel::registerEntry(QSharedPointer<PboEntry_>& entry) {;
        const QSharedPointer<PboEntryCreatedEvent> evt(new PboEntryCreatedEvent(entry.get()));
        emit onEvent(evt.get());
        entries_.push_back(entry);
    }

    void PboModel::registerHeader(QSharedPointer<PboHeader>& header) {
        const QSharedPointer<PboHeaderCreatedEvent> evt(new PboHeaderCreatedEvent(header.get()));
        emit onEvent(evt.get());
        headers_.push_back(header);
    }

    void PboModel::emitLoadBegin(const QString& path) const {
        const QSharedPointer<PboLoadBeginEvent> evt(new PboLoadBeginEvent(path));
        emit onEvent(evt.get());
    }

    void PboModel::emitLoadComplete(const QString& path) const {
        const auto evt = QSharedPointer<PboLoadCompleteEvent>(new PboLoadCompleteEvent(path));
        emit onEvent(evt.get());
    }

    void PboModel::writeFileSignature(const PboHeaderIO& io) const {
        PboEntry eSig = PboEntry::makeSignature();
        io.writeEntry(&eSig);
    }

    void PboModel::writeFileEntries(const PboHeaderIO& io) {
        for (auto i = entries_.begin(); i != entries_.end(); ++i) {
            const QSharedPointer<PboEntry_>& entry = *i;
            assert(entry.get()->fileName.length() > 0);
            if (isDeletePending(entry.get())) {
                QSharedPointer<PboEntryDeleteCompleteEvent> evt(new PboEntryDeleteCompleteEvent(entry.get()));
                emit onEvent(evt.get());
                entries_.erase(i);
            } else if (const QSharedPointer<ChangeMove> move = findPendingMove(entry.get())) {
                auto moved = QSharedPointer<PboEntry_>(new PboEntry_(
                        move->pboFilePath, entry->packingMethod, entry->originalSize, entry->reserved,
                        entry->timestamp, entry->dataSize));
                    moved->dataOffset = entry->dataOffset;
                    io.writeEntry(moved.get());
                    entries_.emplace(i, moved);
            } else {
                io.writeEntry(entry.get());
            }
        }

        assert(!pendingDeletes_.count() && "All the deletes must have been applied");
        assert(!pendingMoves_.count() && "All the moves must have been applied");

        PboEntry eBnd = PboEntry::makeBoundary();
        io.writeEntry(&eBnd);
    }

    bool PboModel::isDeletePending(const PboEntry_* entry) {
        if (pendingDeletes_.contains(entry->fileName)) {
            pendingDeletes_.remove(entry->fileName);
            return true;
        }
        return false;
    }

    QSharedPointer<ChangeMove> PboModel::findPendingMove(const PboEntry_* entry) {
        const auto found = pendingMoves_.find(entry->fileName);
        if (found != pendingMoves_.end()) {
            QSharedPointer<ChangeMove> result;
            found.value().swap(result);
            pendingMoves_.erase(found);
            return result;
        }
        return nullptr;
    }
}
