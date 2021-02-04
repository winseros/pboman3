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
        additions_.push_back(QSharedPointer<ChangeAdd>(new ChangeAdd(systemFilePath, pboFilePath)));
    }

    void PboModel::moveEntry(const PboEntry* entry, const QString& pboFilePath) {
        const auto* ent = dynamic_cast<const PboEntry_*>(entry);
        assert(ent && "Must have a special type");
        transforms_.push_back(QSharedPointer<ChangeMove>(new ChangeMove(ent, pboFilePath)));
    }

    void PboModel::deleteEntry(const PboEntry* entry) {
        const auto* ent = dynamic_cast<const PboEntry_*>(entry);
        assert(ent && "Must have a special type");
        transforms_.push_back(QSharedPointer<ChangeDelete>(new ChangeDelete(ent)));
    }

    void PboModel::updateEntriesOffsets(long offsetStart) {
        for (QSharedPointer<PboEntry_>& entry : entries_) {
            entry->dataOffset = offsetStart;
            offsetStart += entry->dataSize;
        }
    }

    void PboModel::registerEntry(QSharedPointer<PboEntry_>& entry) {
        const auto evt = make_unique<PboEntryUpdatedEvent>(entry.get());
        emit onEvent(evt.get());
        entries_.push_back(move(entry));
    }

    void PboModel::registerHeader(QSharedPointer<PboHeader>& header) {
        const auto evt = make_unique<PboHeaderUpdatedEvent>(header.get());
        emit onEvent(evt.get());
        headers_.push_back(move(header));
    }

    void PboModel::emitLoadBegin(const QString& path) const {
        const auto evt = make_unique<PboLoadBeginEvent>(path);
        emit onEvent(evt.get());
    }

    void PboModel::emitLoadComplete(const QString& path) const {
        const auto evt = QSharedPointer<PboLoadCompleteEvent>(new PboLoadCompleteEvent(path));
        emit onEvent(evt.get());
    }

    void PboModel::emitEntryMoved(const QSharedPointer<PboEntry_>& prevEntry, const QSharedPointer<PboEntry_>& newEntry) const {
        const auto evt = QSharedPointer<PboEntryMovedEvent>(new PboEntryMovedEvent(prevEntry.get(), newEntry.get()));
        emit onEvent(evt.get());
    }

    void PboModel::writeFileSignature(const PboHeaderIO& io) const {
        PboEntry eSig = PboEntry::makeSignature();
        io.writeEntry(&eSig);
    }

    void PboModel::writeFileEntries(const PboHeaderIO& io) {
        for (auto i = entries_.begin(); i != entries_.end(); ++i) {
            const QSharedPointer<PboEntry_>& entry = *i;
            if (const QSharedPointer<ChangeBase> change = getChangeFor(entry.get())) {
                if (const auto* cMove = dynamic_cast<ChangeMove*>(change.get())) {
                    auto moved = QSharedPointer<PboEntry_>(new PboEntry_(
                        cMove->pboFilePath, entry->packingMethod, entry->originalSize, entry->reserved,
                        entry->timestamp, entry->dataSize));
                    moved->dataOffset = entry->dataOffset;
                    io.writeEntry(moved.get());
                    emitEntryMoved(entry, moved);
                    entries_.emplace(i, moved);
                }
                //just skip deleted entries
            } else {
                io.writeEntry(entry.get());
            }
        }

        assert(!transforms_.count() && "All the transforms must have been applied");

        PboEntry eBnd = PboEntry::makeBoundary();
        io.writeEntry(&eBnd);
    }

    QSharedPointer<ChangeBase> PboModel::getChangeFor(const PboEntry_* entry) {
        for (auto i = transforms_.begin(); i != transforms_.end(); ++i) {
            QSharedPointer<ChangeBase>& change = *i;
            if (change->entry == entry) {
                QSharedPointer<ChangeBase> result;
                change.swap(result);
                transforms_.erase(i);
                return result;
            }
        }
        return nullptr;
    }
}
