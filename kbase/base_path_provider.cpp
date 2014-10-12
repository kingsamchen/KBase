
#include "kbase\base_path_provider.h"

#include "kbase\files\file_path.h"

namespace {

}   // namespace

namespace kbase {

FilePath BasePathProvider(PathKey key)
{
    key;
    return FilePath(L"I\\am\\kidding");
}

}   // namespace kbase