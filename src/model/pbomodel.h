#pragma once

#include <memory>
#include <vector>
#include "changestream.h"
#include "pboentry.h"
#include "pboheader.h"
#include "pbomodelevents.h"
#include "io/pbofile.h"
#include "io/pboheaderio.h"

namespace pboman3 {
    using namespace std;

    class PboModel : public QObject {
    Q_OBJECT
    public:
        static PboModel* instance;

        void loadFile(const QString& path);

        void saveFile();

        void createEntry(const QString& systemFilePath, const QString& pboFilePath);

        void moveEntry(const PboEntry* entry, const QString& pboFilePath);

        void deleteEntry(const PboEntry* entry);

    signals:
        void onEvent(const PboModelEvent* event) const;

    private:
        PboModel() = default;

        unique_ptr<PboFile> file_;
        vector<unique_ptr<PboEntry_>> entries_;
        vector<unique_ptr<PboHeader>> headers_;
        vector<unique_ptr<ChangeAdd>> additions_;
        vector<unique_ptr<ChangeBase>> transforms_;

        void updateEntriesOffsets(long offsetStart);

        void registerEntry(unique_ptr<PboEntry_>& entry);

        void registerHeader(unique_ptr<PboHeader>& header);

        void emitLoadBegin(const QString& path) const;

        void emitLoadComplete(const QString& path) const;

        void emitEntryMoved(const unique_ptr<PboEntry_>& prevEntry, const unique_ptr<PboEntry_>& newEntry) const;

        void writeFileSignature(const PboHeaderIO& io) const;

        void writeFileEntries(const PboHeaderIO& io);

        unique_ptr<ChangeBase> getChangeFor(const PboEntry_* entry);
    };
}
