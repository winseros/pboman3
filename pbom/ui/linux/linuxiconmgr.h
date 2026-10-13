#pragma once

#include <QHash>
#include "ui/iconmgr.h"

namespace pboman3::ui {
    class LinuxIconMgr final : public IconMgr {
    public:
        LinuxIconMgr();

        const QIcon& getIconForExtension(const QString& extension) override;

        const QIcon& getFolderOpenedIcon() override;

        const QIcon& getFolderClosedIcon() override;

    private:
        QHash<QString, QIcon> cache_;
    };
}
