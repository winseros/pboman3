#pragma once

#include <vector>
#include <filesystem>
#include <ShObjIdl.h>

namespace pboman3 {
    using namespace std;
    using namespace filesystem;

    vector<path> GetSelectedItemsPaths(IShellItemArray* shellItems);
}
