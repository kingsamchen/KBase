/*
 @ Kingsley Chen
*/

#include "kbase\files\file_enumerator.h"

#include "kbase\error_exception_util.h"

namespace kbase {

FileEnumerator::FileEnumerator(const FilePath& root_path, bool recursive,
                               int file_type)
 : FileEnumerator(root_path, recursive, file_type, PathString())
{}

FileEnumerator::FileEnumerator(const FilePath& root_path,
                               bool recursive,
                               int file_type,
                               const PathString& pattern)
 : root_path_(root_path), recursive_(recursive), file_type_(file_type),
   pattern_(pattern), find_handle_(INVALID_HANDLE_VALUE), has_find_data_(false)
{
    memset(&find_data_, 0, sizeof(find_data_));
    pending_paths_.push(root_path);
}

FilePath FileEnumerator::Next()
{
    while (!pending_paths_.empty() || has_find_data_) {
        if (!has_find_data_) {
            root_path_ = pending_paths_.top();
            pending_paths_.pop();

            FilePath cur_root = root_path_;
            if (pattern_.empty()) {
                // If no pattern was specified, matches everything by default.
                cur_root.Append(L"*");
            } else {
                cur_root.Append(pattern_);
            }

            find_handle_.Reset(FindFirstFileExW(cur_root.value().c_str(),
                                                FindExInfoBasic,
                                                &find_data_,
                                                FindExSearchNameMatch,
                                                nullptr,
                                                FIND_FIRST_EX_LARGE_FETCH));
            has_find_data_ = true;
        } else {
            // Enumeration in this directory is accomplished.
            if (!FindNextFileW(find_handle_, &find_data_)) {
                FindClose(find_handle_);
                find_handle_ = nullptr;
            }
        }

        if (!find_handle_) {
            has_find_data_ = false;
            continue;
        }

        FilePath cur_file(find_data_.cFileName);
        if (ShouldSkip(cur_file)) {
            continue;
        }

        // Now is in absolute path.
        cur_file = root_path_;
        cur_file.Append(find_data_.cFileName);

        if (find_data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (recursive_) {
                // Ignores symbolic link directories in case we stick in an infinite
                // cycle.
                if (!(find_data_.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                    pending_paths_.push(cur_file);
                }
            }

            if (file_type_ & FileType::DIRS) {
                return cur_file;
            }
        } else if (file_type_ & FileType::FILES) {
            return cur_file;
        }
    }
    
    return FilePath();
}

FileInfo FileEnumerator::GetInfo() const
{
    ENSURE(has_find_data_).raise();

    ULARGE_INTEGER file_size;
    file_size.LowPart = find_data_.nFileSizeLow;
    file_size.HighPart = find_data_.nFileSizeHigh;

    return FileInfo(find_data_.cFileName,
                    static_cast<int64_t>(file_size.QuadPart),
                    !!(find_data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY),
                    DateTime(find_data_.ftCreationTime),
                    DateTime(find_data_.ftLastWriteTime),
                    DateTime(find_data_.ftLastAccessTime));
}

bool FileEnumerator::ShouldSkip(const FilePath& path)
{
    PathString base_name = path.BaseName().value();
    if (base_name == FilePath::kCurrentDir || base_name == FilePath::kParentDir) {
        return true;
    }

    return false;
}

}   // namespace kbase