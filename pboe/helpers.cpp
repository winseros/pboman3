#include "helpers.h"

namespace pboman3 {
    vector<path> GetSelectedItemsPaths(IShellItemArray* shellItems) {
        vector<path> result;

        DWORD count;
        const HRESULT hr = shellItems->GetCount(&count);
        if (FAILED(hr) || count == 0)
            return result;

        result.reserve(count);

        for (DWORD i = 0; i < count; i++) {
            IShellItem* item;
            shellItems->GetItemAt(i, &item);

            LPWSTR buffer;
            item->GetDisplayName(SIGDN_FILESYSPATH, &buffer);
            result.emplace_back(buffer);

            item->Release();
            CoTaskMemFree(buffer);
        }

        return result;
    }
}
