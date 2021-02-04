#pragma once

#include <QList>
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

        QSharedPointer<PboFile> file_;
        QList<QSharedPointer<PboEntry_>> entries_;
        QList<QSharedPointer<PboHeader>> headers_;
        QList<QSharedPointer<ChangeAdd>> additions_;
        QList<QSharedPointer<ChangeBase>> transforms_;

        void updateEntriesOffsets(long offsetStart);

        void registerEntry(QSharedPointer<PboEntry_>& entry);

        void registerHeader(QSharedPointer<PboHeader>& header);

        void emitLoadBegin(const QString& path) const;

        void emitLoadComplete(const QString& path) const;

        void emitEntryMoved(const QSharedPointer<PboEntry_>& prevEntry, const QSharedPointer<PboEntry_>& newEntry) const;

        void writeFileSignature(const PboHeaderIO& io) const;

        void writeFileEntries(const PboHeaderIO& io);

        QSharedPointer<ChangeBase> getChangeFor(const PboEntry_* entry);
    };
}
