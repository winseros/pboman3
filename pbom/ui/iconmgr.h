#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <QIcon>

namespace pboman3 {
    class IconMgr {
    public:
        virtual ~IconMgr() = default;

        virtual const QIcon& getIconForExtension(const QString& extension) = 0;

        virtual const QIcon& getFolderOpenedIcon() = 0;

        virtual const QIcon& getFolderClosedIcon() = 0;
    };
}
