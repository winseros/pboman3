#pragma once

#include <QObject>
#include "vector"
#include <memory>
#include "pboentry.h"
#include "pboheader.h"
#include "pbomodelevents.h"
#include "io/pbofile.h"

namespace pboman3 {
    using namespace std;

    class PboModel : public QObject {
    Q_OBJECT
    public:

        static PboModel* instance;

        void loadFile(const QString& path);

    signals:
        void onEvent(const PboModelEvent* event) const;

    private:
        PboModel() = default;

        unique_ptr<PboFile> file_;
        vector<unique_ptr<PboEntry_>> entries_;
        vector<unique_ptr<PboHeader>> headers_;

        void updateEntriesOffsets(long offsetStart);

        void registerEntry(unique_ptr <PboEntry_>& entry);

        void registerHeader(unique_ptr <PboHeader>& header);

        void emitLoadBegin(const QString& path) const;

        void emitLoadComplete(const QString& path) const;
    };
}