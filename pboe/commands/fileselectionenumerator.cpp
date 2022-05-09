#include <wrl/implements.h>
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


    ComPtr<IExplorerCommand> FileSelectionEnumerator::createForIndex(ULONG index) const {
        if (index == 0) {
            return Make<UnpackToCommand>(executable_, selectedFiles_);
        } else if (index == 1) {
            if (selectedFiles_->size() == 1)
                return Make<UnpackAsCommand>(executable_, selectedFiles_->at(0));
            else
                return Make<UnpackInCommand>(executable_, selectedFiles_);
        }

        return NULL;
    }

    ULONG FileSelectionEnumerator::numberOfItems() const {
        return 2;
    }
}
