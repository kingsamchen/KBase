/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILES_FILE_ENUMERATOR_H_
#define KBASE_FILES_FILE_ENUMERATOR_H_

#include <stack>

#include <windows.h>

#include "kbase/files/file_path.h"

namespace kbase {

class FileEnumerator {
public:
    class FileInfo {
    
    };

    enum FileType {
        FILES = 0x1,
        DIRS = 0x2,
    };

    FileEnumerator(const FilePath& root_path, bool recursive, FileType file_type);

    FileEnumerator(const FilePath& root_path, bool recursive, FileType file_type,
                   const FilePath::PathString& pattern);

    ~FileEnumerator();
    
    FileEnumerator(const FileEnumerator&) = delete;
    FileEnumerator& operator=(const FileEnumerator&) = delete;

    FilePath Next();

    FileInfo GetInfo() const;

private:
    // Returns true, if the filename in path should be skipped according to the
    // specified policies.
    // Returns false, otherwise.
    // This function skips '.' and '..'.
    bool ShouldSkip(const FilePath& path);

private:
    FilePath root_path_;
    bool recursive_;
    FileType file_type_;
    FilePath::PathString pattern_;
    WIN32_FIND_DATA find_data_;
    HANDLE find_handle_;
    bool has_find_data_;
    std::stack<FilePath> pending_paths_;
};

}   // namespace kbase

#endif  // KBASE_FILES_FILE_ENUMERATOR_H_