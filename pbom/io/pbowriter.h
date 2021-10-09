#pragma once

#include <QHash>
#include "pbofile.h"
#include "bs/pbobinarysource.h"
#include "model/headersmodel.h"
#include "model/pboentry.h"
#include "model/pbonode.h"

namespace pboman3 {
    class PboWriter : public QObject {
        Q_OBJECT

    public:
        struct ProgressEvent;

        PboWriter();

        PboWriter& usePath(QString path);

        PboWriter& useHeaders(HeadersModel* headers);

        PboWriter& useRoot(PboNode* root);

        PboWriter& copySignatureTo(QByteArray* signature);

        void write(const Cancel& cancel);

        void suspendBinarySources() const;

        void resumeBinarySources() const;

        void assignBinarySources(const QString& path);

    signals:
        void progress(const ProgressEvent* evt);

    private:
        QString path_;
        PboNode* root_;
        HeadersModel* headers_;
        QByteArray* signature_;
        QHash<PboNode*, PboDataInfo> binarySources_;

        void writeNode(QFileDevice* file, PboNode* node, QList<PboEntry>& entries, const Cancel& cancel);

        void writeHeader(PboFile* file, const QList<PboEntry>& entries, const Cancel& cancel);

        void copyBody(QFileDevice* pbo, QFileDevice* body, const Cancel& cancel);

        void writeSignature(QFileDevice* pbo, const Cancel& cancel);

        void suspendBinarySources(PboNode* node) const;

        void resumeBinarySources(PboNode* node) const;

        void assignBinarySources(PboNode* node, const QString& path);

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
            CopyBytesEvent(qsizetype c, qsizetype t):
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
