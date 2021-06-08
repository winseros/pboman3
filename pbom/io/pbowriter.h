#pragma once

#include <QHash>
#include "pbofile.h"
#include "bs/pbobinarysource.h"
#include "model/headersmodel.h"
#include "model/pboentry.h"
#include "model/pbonode.h"

namespace pboman3 {
    class PboWriter {
    public:
        PboWriter();

        PboWriter& usePath(QString path);

        PboWriter& useHeaders(HeadersModel* headers);

        PboWriter& useRoot(PboNode* root);

        void write(const Cancel& cancel);

        void cleanBinarySources() const;

        void reassignBinarySources(const QString& path);

    private:
        QString path_;
        PboNode* root_;
        HeadersModel* headers_;
        QHash<PboNode*, PboDataInfo> binarySources_;

        void writeNode(QFileDevice* file, PboNode* node, QList<PboEntry>& entries, const Cancel& cancel);

        void writeHeader(PboFile* file, const QList<PboEntry>& entries, const Cancel& cancel);

        void copyBody(QFileDevice* pbo, QFileDevice* body, const Cancel& cancel);

        void cleanBinarySources(PboNode* node) const;

        void reassignBinarySources(PboNode* node, const QString& path);
    };
}
