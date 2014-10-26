
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILES_FILE_UTIL_H_
#define KBASE_FILES_FILE_UTIL_H_

#include "kbase\files\file_info.h"
#include "kbase\files\file_path.h"

namespace kbase {

// Returns an absolute or full path name of the relative |path|.
// Returns an empty path on error.
FilePath MakeAbsoluteFilePath(const FilePath& path);

// Returns true if the path exists.
// Returns false if doesn't exist.
bool PathExists(const FilePath& path);

// Retrieves the information of a given file or directory.
// Throws an exception when error occurs.
// Be wary of that the size-field of the |FileInfo| is valid if and only if the
// |path| indicates a file.
FileInfo GetFileInfo(const FilePath& path);

void Delete(const FilePath& path, bool recursive);

void DeleteFileAfterReboot(const FilePath& path);

void Move(const FilePath& src, const FilePath& dest);

}   // namespace kbase

#endif  // KBASE_FILES_FILE_UTIL_H_