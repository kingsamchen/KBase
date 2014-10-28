
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

// Returns true if the directory exists.
// Returns false, otherwise.
bool DirectoryExists(const FilePath& path);

// Retrieves the information of a given file or directory.
// Throws an exception when error occurs.
// Be wary of that the size-field of the |FileInfo| is valid if and only if the
// |path| indicates a file.
FileInfo GetFileInfo(const FilePath& path);

// Removes a file or a directory indicated by the given |path|.
// If want to remove a directory non-recursively, the directory must be empty.
// Throws an exception when failed.
void RemoveFile(const FilePath& path, bool recursive);

// If the |path| refers to a directory, the system removes the directory only if the
// directory is empty.
// This function only marks the file or directory as should-be-deleted, it doesn't
// guarantee the deletion will be enforced.
// Throws an exception when failed to mark.
void RemoveFileAfterReboot(const FilePath& path);

// Copies a single file from |src| to |dest|.
// If |dest| already exists, has it overwritten.
// The file copied retains file attribute from the source.
void DuplicateFile(const FilePath& src, const FilePath& dest);

// Copy all files in |src| to |dest| if recursive is false.
// Copy all content, including subfolders in |src| to |dest| if recursive is true.
// Overwrites files that already exist.
void DuplicateDirectory(const FilePath& src, const FilePath& dest, bool recursive);

void MoveFile(const FilePath& src, const FilePath& dest);

}   // namespace kbase

#endif  // KBASE_FILES_FILE_UTIL_H_