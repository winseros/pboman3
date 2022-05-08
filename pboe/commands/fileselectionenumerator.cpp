#include "fileselectionenumerator.h"
#include "unpackascommand.h"
#include "unpackincommand.h"
#include "unpacktocommand.h"

namespace pboman3 {
    FileSelectionEnumerator::FileSelectionEnumerator(shared_ptr<Executable> executable,
                                                     shared_ptr<vector<path>> selectedFiles)
        : executable_(move(executable)),
          selectedFiles_(move(selectedFiles)) {
    }


    IExplorerCommand* FileSelectionEnumerator::createForIndex(ULONG index) const {
        IExplorerCommand* command = NULL;

        if (index == 0) {
            command = new UnpackToCommand(executable_, selectedFiles_);
        } else if (index == 1) {
            if (selectedFiles_->size() == 1)
                command = new UnpackAsCommand(executable_, selectedFiles_->at(0));
            else
                command = new UnpackInCommand(executable_, selectedFiles_);
        }

        return command;
    }

    ULONG FileSelectionEnumerator::numberOfItems() const {
        return 2;
    }
}
