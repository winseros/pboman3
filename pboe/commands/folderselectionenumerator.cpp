#include "folderselectionenumerator.h"
#include "packtocommand.h"
#include <filesystem>
#include "packascommand.h"
#include "packincommand.h"

namespace pboman3 {
    FolderSelectionEnumerator::FolderSelectionEnumerator(shared_ptr<Executable> executable,
                                                         shared_ptr<vector<std::filesystem::path>> selectedFolders)
        : executable_(move(executable)),
          selectedFolders_(move(selectedFolders)) {
    }

    IExplorerCommand* FolderSelectionEnumerator::createForIndex(ULONG index) const {
        IExplorerCommand* command = NULL;

        if (index == 0) {
            command = new PackToCommand(executable_, selectedFolders_);
        } else if (index == 1) {
            if (selectedFolders_->size() == 1)
                command = new PackAsCommand(executable_, selectedFolders_->at(0));
            else
                command = new PackInCommand(executable_, selectedFolders_);
        }

        return command;
    }

    ULONG FolderSelectionEnumerator::numberOfItems() const {
        return 2;
    }
}
