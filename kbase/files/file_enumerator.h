/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILES_FILE_ENUMERATOR_H_
#define KBASE_FILES_FILE_ENUMERATOR_H_

#include <cstdint>
#include <stack>

#include <windows.h>

#include "kbase\basic_types.h"
#include "kbase\date_time.h"
#include "kbase\files\file_info.h"
#include "kbase\files\file_path.h"
#include "kbase\memory\scoped_handle.h"

namespace {

struct FindHandleTraits : public kbase::HandleTraits<HANDLE> {
    FindHandleTraits() = delete;    
    ~FindHandleTraits() = delete;

    static void Close(Handle handle)
    {
        FindClose(handle);
    }
};

typedef kbase::ScopedHandle<HANDLE, FindHandleTraits> FileFindHandle;

}   // namespace

namespace kbase {

class FileEnumerator {
public:
    enum FileType :int {
        FILES = 1 << 0,
        DIRS  = 1 << 1
    };

    // If |recursive| is true, it will enumerate all matches in every subdirectories.
    // It employes BFS to enumerate matched entries.
    FileEnumerator(const FilePath& root_path, bool recursive, int file_type);
    FileEnumerator(const FilePath& root_path, bool recursive, int file_type,
                   const PathString& pattern);

    ~FileEnumerator() = default;
    
    FileEnumerator(const FileEnumerator&) = delete;
    FileEnumerator& operator=(const FileEnumerator&) = delete;

    FilePath Next();

    // Gets the information of the current file.
    // If there currently is no file, the function throws an exception.
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
    int file_type_;
    PathString pattern_;
    WIN32_FIND_DATA find_data_;
    FileFindHandle find_handle_;
    bool has_find_data_;
    std::stack<FilePath> pending_paths_;
};

}   // namespace kbase

#endif  // KBASE_FILES_FILE_ENUMERATOR_H_