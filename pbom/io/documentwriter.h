#pragma once

#include <QHash>
#include <QString>
#include "pboentry.h"
#include "pbofile.h"
#include "bs/pbobinarysource.h"
#include "domain/pbodocument.h"
#include "util/util.h"

namespace pboman3::io {
    using namespace domain;

    class DocumentWriter: public QObject {
        Q_OBJECT

    public:
        DocumentWriter(QString path);

        void write(PboDocument* document, const Cancel& cancel);

        struct ProgressEvent;

    signals:
        void progress(const ProgressEvent* evt);

    private:
        QString path_;
        QHash<DocumentNode*, PboDataInfo> binarySources_;

        void writeInternal(PboDocument* document, const QString& path, const Cancel& cancel);

        void writeNode(QFileDevice* file, DocumentNode* node, QList<QSharedPointer<PboEntry>>& entries, const Cancel& cancel);

        void writeHeader(PboFile* file, const DocumentHeaders* headers, const QList<QSharedPointer<PboEntry>>& entries, const Cancel& cancel);

        void copyBody(QFileDevice* pbo, QFileDevice* body, const Cancel& cancel);

        void writeSignature(QFileDevice* pbo, PboDocument* document, const Cancel& cancel);

        void suspendBinarySources(DocumentNode* node) const;

        void resumeBinarySources(DocumentNode* node) const;

        void assignBinarySources(DocumentNode* node);

        void emitWriteEntry();

        void emitCopyBytes(qsizetype copied, qsizetype total);

        void emitCalcHash(qsizetype processed, qsizetype total);

    public:
        struct ProgressEvent {
            virtual ~ProgressEvent() = default;
        };

        struct WriteEntryEvent : ProgressEvent {
        };

        struct CopyBytesEvent : ProgressEvent {
            CopyBytesEvent(qsizetype c, qsizetype t) :
                copied(c), total(t) {
            }

            const qsizetype copied;
            const qsizetype total;
        };

        struct CalcHashEvent : ProgressEvent {
            CalcHashEvent(qsizetype p, qsizetype t) :
                processed(p), total(t) {
            }

            const qsizetype processed;
            const qsizetype total;
        };
    };
}
