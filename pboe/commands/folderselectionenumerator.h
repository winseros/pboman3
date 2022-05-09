#pragma once

#include <filesystem>
#include "abstractenumerator.h"
#include "../executable.h"

namespace pboman3 {
    using namespace std;
    using namespace filesystem;

    class FolderSelectionEnumerator final : public AbstractEnumerator<> {
    public:
        FolderSelectionEnumerator(shared_ptr<Executable> executable,
                                  shared_ptr<vector<path>> selectedFolders);

    protected:
        ComPtr<IExplorerCommand> createForIndex(ULONG index) const override;

        ULONG numberOfItems() const override;

    private:
        shared_ptr<Executable> executable_;
        shared_ptr<vector<path>> selectedFolders_;
    };
}
