#pragma once

#include "unpackbackend.h"

namespace pboman3 {
    class UnpackTaskBackend
        : public UnpackBackend {

    public:
        UnpackTaskBackend(const QDir& folder);

        void setOnError(std::function<void(const QString&)>* callback);

        void setOnProgress(std::function<void()>* callback);

    protected:
        void unpackFileNode(const PboNode* rootNode, const PboNode* childNode, const Cancel& cancel) const override;

    private:
        std::function<void(const QString&)>* onError_;
        std::function<void()>* onProgress_;

        void error(const QString& error) const;

        void progress() const;
    };
}
