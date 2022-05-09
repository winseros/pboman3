#pragma once

#include <vector>
#include <filesystem>
#include "abstractenumerator.h"
#include "../executable.h"

namespace pboman3 {
    using namespace std;
    using namespace filesystem;

    class FileSelectionEnumerator : public AbstractEnumerator<> {
    public:
        FileSelectionEnumerator(shared_ptr<Executable> executable,
                                shared_ptr<vector<path>> selectedFiles);

    protected:
        ComPtr<IExplorerCommand> createForIndex(ULONG index) const override;

        ULONG numberOfItems() const override;

    private:
        shared_ptr<Executable> executable_;
        shared_ptr<vector<path>> selectedFiles_;
    };
}
