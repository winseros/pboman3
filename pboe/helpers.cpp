#include "helpers.h"

namespace pboman3 {
    vector<path> GetSelectedItemsPaths(IShellItemArray* shellItems) {
        vector<path> result;

        DWORD count;
        HRESULT hr = shellItems->GetCount(&count);
        if (FAILED(hr) || count == 0)
            return result;

        result.reserve(count);

        for (DWORD i = 0; i < count; i++) {
            IShellItem* item;
            shellItems->GetItemAt(i, &item);

            LPWSTR buffer;
            hr = item->GetDisplayName(SIGDN_FILESYSPATH, &buffer);
            if (SUCCEEDED(hr))
                result.emplace_back(buffer);

            item->Release();
            CoTaskMemFree(buffer);
        }

        return result;
    }
}
