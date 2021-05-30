#pragma once

#include "pbofile.h"
#include "model/pboentry.h"
#include "model/pboheader.h"
#include "model/pbonode.h"

namespace pboman3 {
    class PboWriter {
    public:
        PboWriter();

        PboWriter& usePath(QString path);

        PboWriter& addHeader(PboHeader* header);

        PboWriter& useRoot(PboNode* root);

        void write(const Cancel& cancel);

    private:
        QString path_;
        QList<PboHeader*> headers_;
        PboNode* root_;

        void writeNode(QFileDevice* file, PboNode* node, QList<PboEntry>& entries, const Cancel& cancel);

        void writeHeader(PboFile* file, const QList<PboEntry>& entries, const Cancel& cancel);

        void copyBody(QFileDevice* pbo, QFileDevice* body, const Cancel& cancel);
    };
}
