#include "pbomodel.h"
#include <QDir>
#include <QScopedPointer>
#include <QUuid>
#include <algorithm>
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

    void PboModel::scheduleEntryMove(const PboEntry* entry, const QString& pboFilePath) {
        if (!findPendingDelete(entry, false) && isEntryRegistered(entry)) {
            const auto moved = QSharedPointer<PboEntry_>(new PboEntry_(
                pboFilePath, entry->packingMethod, entry->originalSize, entry->reserved,
                entry->timestamp, entry->dataSize));
            const auto* ent = dynamic_cast<const PboEntry_*>(entry);
            assert(ent && "The entry must be created by the pbomodel, not by hand");
            moved->dataOffset = ent->dataOffset;

            if (const auto existingMove = findPendingMove(entry)) {
                const QScopedPointer<PboEntryMoveCanceledEvent> evt(new PboEntryMoveCanceledEvent(entry, existingMove.get()));
                emit onEvent(evt.get());
            }
            pendingMoves_.insert(entry->fileName, moved);
            const QScopedPointer<PboEntryMoveScheduledEvent> evt(new PboEntryMoveScheduledEvent(entry, moved.get()));
            emit onEvent(evt.get());
        }
    }

    void PboModel::cancelEntryMove(const PboEntry* entry) {
        if (isEntryRegistered(entry)) {
            if (const auto existingMove = findPendingMove(entry)) {
                const QScopedPointer<PboEntryMoveCanceledEvent> evt(new PboEntryMoveCanceledEvent(entry, existingMove.get()));
                emit onEvent(evt.get());
            }
        }
    }

    void PboModel::scheduleEntryDelete(const PboEntry* entry) {
        if (!pendingDeletes_.contains(entry->fileName) && isEntryRegistered(entry)) {
            pendingDeletes_.insert(entry->fileName);
            const QScopedPointer<PboEntryDeleteScheduledEvent> evt(new PboEntryDeleteScheduledEvent(entry));
            emit onEvent(evt.get());
        }
    }

    void PboModel::cancelEntryDelete(const PboEntry* entry) {
        if (pendingDeletes_.remove(entry->fileName)) {
            const QScopedPointer<PboEntryDeleteCanceledEvent> evt(new PboEntryDeleteCanceledEvent(entry));
            emit onEvent(evt.get());
        }
    }

    void PboModel::updateEntriesOffsets(long offsetStart) {
        for (QSharedPointer<PboEntry_>& entry : entries_) {
            entry->dataOffset = offsetStart;
            offsetStart += entry->dataSize;
        }
    }

    void PboModel::registerEntry(QSharedPointer<PboEntry_>& entry) {
        const QScopedPointer<PboEntryCreatedEvent> evt(new PboEntryCreatedEvent(entry.get()));
        emit onEvent(evt.get());
        entries_.append(entry);
    }

    void PboModel::registerHeader(QSharedPointer<PboHeader>& header) {
        const QScopedPointer<PboHeaderCreatedEvent> evt(new PboHeaderCreatedEvent(header.get()));
        emit onEvent(evt.get());
        headers_.append(header);
    }

    void PboModel::emitLoadBegin(const QString& path) const {
        const QScopedPointer<PboLoadBeginEvent> evt(new PboLoadBeginEvent(path));
        emit onEvent(evt.get());
    }

    void PboModel::emitLoadComplete(const QString& path) const {
        const auto evt = QScopedPointer<PboLoadCompleteEvent>(new PboLoadCompleteEvent(path));
        emit onEvent(evt.get());
    }

    void PboModel::writeFileSignature(const PboHeaderIO& io) const {
        PboEntry eSig = PboEntry::makeSignature();
        io.writeEntry(&eSig);
    }

    void PboModel::writeFileEntries(const PboHeaderIO& io) {
        auto it = entries_.begin();
        while (it != entries_.end()) {
            const QSharedPointer<PboEntry_>& entry = *it;
            assert(entry.get()->fileName.length() > 0);
            if (findPendingDelete(entry.get(), true)) {
                const QScopedPointer<PboEntryDeleteCompleteEvent> evt(new PboEntryDeleteCompleteEvent(entry.get()));
                emit onEvent(evt.get());
                it = entries_.erase(it);
            } else if (const QSharedPointer<PboEntry_> moved = findPendingMove(entry.get())) {
                const QScopedPointer<PboEntryMoveCompleteEvent> evt(new PboEntryMoveCompleteEvent(it->get(), moved.get()));
                emit onEvent(evt.get());
                io.writeEntry(moved.get());
                it = entries_.emplace(it, moved);
            } else {
                io.writeEntry(entry.get());
                ++it;
            }
        }

        assert(!pendingDeletes_.count() && "All the deletes must have been applied");
        assert(!pendingMoves_.count() && "All the moves must have been applied");

        const PboEntry eBnd = PboEntry::makeBoundary();
        io.writeEntry(&eBnd);
    }

    bool PboModel::findPendingDelete(const PboEntry* entry, const bool pop) {
        if (pendingDeletes_.contains(entry->fileName)) {
            if (pop) {
                pendingDeletes_.remove(entry->fileName);
            }
            return true;
        }
        return false;
    }

    bool PboModel::isEntryRegistered(const PboEntry* entry) {
        const QString& fn = entry->fileName;
        return std::any_of(entries_.begin(), entries_.end(),
                           [&fn](const QSharedPointer<PboEntry>& e) {
                               return e->fileName == fn;
                           });
    }

    QSharedPointer<PboEntry_> PboModel::findPendingMove(const PboEntry* entry) {
        const auto found = pendingMoves_.find(entry->fileName);
        if (found != pendingMoves_.end()) {
            QSharedPointer<PboEntry_> result;
            found.value().swap(result);
            pendingMoves_.erase(found);
            return result;
        }
        return nullptr;
    }
}
