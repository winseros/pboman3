#include "pbomodel.h"
#include <memory>
#include "io/pboheaderio.h"

namespace pboman3 {
    using namespace std;

    PboModel* PboModel::instance = new PboModel;

    void PboModel::loadFile(const QString& path) {
        if (file_) {
            file_->close();
        }

        file_ = make_unique<PboFile>(path);
        file_->open(QIODeviceBase::OpenModeFlag::ReadWrite);
        emitLoadBegin(path);

        PboHeaderIO reader(file_.get());
        unique_ptr<PboEntry_> entry = reader.readNextEntry();

#define BAIL file_->close(); \
        unique_ptr<PboLoadFailedEvent> evt = make_unique<PboLoadFailedEvent>(); \
        emit onEvent(evt.get()); \
        return;

        if (!entry) { BAIL }

        if (entry->isSignature()) {
            unique_ptr<PboHeader> header = reader.readNextHeader();
            while (header && !header->isBoundary()) {
                registerHeader(header);
                header = reader.readNextHeader();
            }
            if (!header) { BAIL }
        } else if (entry->isContent()) {
            registerEntry(entry);
        } else {
            BAIL
        }

        entry = reader.readNextEntry();
        while (entry && entry->isContent()) {
            registerEntry(entry);
            entry = reader.readNextEntry();
        }
        if (!entry || !entry->isBoundary()) { BAIL }

        updateEntriesOffsets(file_->pos());
        emitLoadComplete(path);
    }

    void PboModel::updateEntriesOffsets(long offsetStart) {
        for (unique_ptr<PboEntry_>& entry: entries_) {
            entry->dataOffset = offsetStart;
            offsetStart += entry->dataSize;
        }
    }

    void PboModel::registerEntry(unique_ptr<PboEntry_>& entry) {
        auto evt = make_unique<PboEntryUpdatedEvent>(entry.get());
        emit onEvent(evt.get());
        entries_.push_back(std::move(entry));
    }

    void PboModel::registerHeader(unique_ptr<PboHeader>& header) {
        auto evt = make_unique<PboHeaderUpdatedEvent>(header.get());
        emit onEvent(evt.get());
        headers_.push_back(std::move(header));
    }

    void PboModel::emitLoadBegin(const QString& path) const {
        auto evt = make_unique<PboLoadBeginEvent>(path);
        emit onEvent(evt.get());
    }

    void PboModel::emitLoadComplete(const QString& path) const {
        auto evt = make_unique<PboLoadCompleteEvent>(path);
        emit onEvent(evt.get());
    }
}