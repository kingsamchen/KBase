
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILES_FILE_UTIL_H_
#define KBASE_FILES_FILE_UTIL_H_

#include "kbase\files\file_path.h"

namespace kbase {

// Returns an absolute or full path name of the relative |path|.
// Returns an empty path on error.
FilePath MakeAbsoluteFilePath(const FilePath& path);

}   // namespace kbase

#endif  // KBASE_FILES_FILE_UTIL_H_