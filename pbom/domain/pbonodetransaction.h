#pragma once

#include "pbonode.h"

namespace pboman3::domain {
    class PboNodeTransaction {
    public:
        PboNodeTransaction(PboNode* node);

        ~PboNodeTransaction();

        const QString& title() const;

        void setTitle(QString title);

        void commit();
    private:
        bool committed_;
        PboNode* node_;
        QString title_;

        QString verifyTitle(const QString& title) const;
    };
}
