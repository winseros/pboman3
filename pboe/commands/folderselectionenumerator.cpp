#include <wrl/implements.h>
#include <filesystem>
#include "folderselectionenumerator.h"
#include "packtocommand.h"
#include "packascommand.h"
#include "packincommand.h"

namespace pboman3 {
    FolderSelectionEnumerator::FolderSelectionEnumerator(shared_ptr<Executable> executable,
                                                         shared_ptr<vector<std::filesystem::path>> selectedFolders)
        : executable_(move(executable)),
          selectedFolders_(move(selectedFolders)) {
    }

    ComPtr<IExplorerCommand> FolderSelectionEnumerator::createForIndex(ULONG index) const {
        if (index == 0) {
            return Make<PackToCommand>(executable_, selectedFolders_);
        } else if (index == 1) {
            if (selectedFolders_->size() == 1)
                return Make<PackAsCommand>(executable_, selectedFolders_->at(0));
            else
                return Make<PackInCommand>(executable_, selectedFolders_);
        }

        return NULL;
    }

    ULONG FolderSelectionEnumerator::numberOfItems() const {
        return 2;
    }
}
