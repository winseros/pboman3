#pragma once

#include <QList>
#include <QMap>
#include <QSet>
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

        void scheduleEntryDelete(const PboEntry* entry);

        void cancelEntryDelete(const PboEntry* entry);

    signals:
        void onEvent(const PboModelEvent* event) const;

    private:
        PboModel() = default;

        QSharedPointer<PboFile> file_;
        QList<QSharedPointer<PboEntry_>> entries_;
        QList<QSharedPointer<PboHeader>> headers_;
        QList<QSharedPointer<ChangeAdd>> pendingAdds_;
        QMap<QString, QSharedPointer<ChangeMove>> pendingMoves_;
        QSet<QString> pendingDeletes_;

        void updateEntriesOffsets(long offsetStart);

        void registerEntry(QSharedPointer<PboEntry_>& entry);

        void registerHeader(QSharedPointer<PboHeader>& header);

        void emitLoadBegin(const QString& path) const;

        void emitLoadComplete(const QString& path) const;

        void writeFileSignature(const PboHeaderIO& io) const;

        void writeFileEntries(const PboHeaderIO& io);

        bool isDeletePending(const PboEntry_* entry);

        QSharedPointer<ChangeMove> findPendingMove(const PboEntry_* entry);
    };
}
