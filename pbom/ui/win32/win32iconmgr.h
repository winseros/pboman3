#pragma once

#include "ui/iconmgr.h"
#include <QHash>

namespace pboman3 {
    class Win32IconMgr: public IconMgr {
    public:
        Win32IconMgr();

        const QIcon& getIconForExtension(const QString& extension) override;

        const QIcon& getFolderOpenedIcon() override;

        const QIcon& getFolderClosedIcon() override;

    private:
        QHash<QString, QIcon> cache_;
    };
}
