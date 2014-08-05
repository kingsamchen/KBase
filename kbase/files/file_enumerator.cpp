/*
 @ Kingsley Chen
*/

#include "kbase/files/file_enumerator.h"

#include <cassert>
#include <stdexcept>

#include "kbase/version_util.h"

namespace kbase {

FileEnumerator::FileEnumerator(const FilePath& root_path, bool recursive,
                               FileType file_type)
 : FileEnumerator(root_path, recursive, file_type, FilePath::PathString())
{}

FileEnumerator::FileEnumerator(const FilePath& root_path,
                               bool recursive,
                               FileType file_type,
                               const FilePath::PathString& pattern)
 : root_path_(root_path), recursive_(recursive), file_type_(file_type),
   pattern_(pattern), find_handle_(INVALID_HANDLE_VALUE), has_find_data_(false)
{
    memset(&find_data_, 0, sizeof(find_data_));
    pending_paths_.push(root_path);
}

FileEnumerator::~FileEnumerator()
{
    if (find_handle_ != INVALID_HANDLE_VALUE) {
        FindClose(find_handle_);
    }
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

            if (OSInfo::GetInstance()->IsVersionOrGreater(OSInfo::WIN_7)) {
                find_handle_ = FindFirstFileEx(cur_root.value().c_str(),
                                               FindExInfoBasic,
                                               &find_data_,
                                               FindExSearchNameMatch,
                                               nullptr,
                                               FIND_FIRST_EX_LARGE_FETCH);
            } else {
                find_handle_ = FindFirstFile(cur_root.value().c_str(), &find_data_);
            }

            has_find_data_ = true;
        } else {
            // Enumeration in this directory is accomplished.
            if (!FindNextFile(find_handle_, &find_data_)) {
                FindClose(find_handle_);
                find_handle_ = INVALID_HANDLE_VALUE;
            }
        }

        if (find_handle_ == INVALID_HANDLE_VALUE) {
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

FileEnumerator::FileInfo FileEnumerator::GetInfo() const
{
    assert(has_find_data_);
    if (!has_find_data_) {
        throw std::logic_error("try to get file info without data!");   
    }

    FileInfo file_info;
    memcpy_s(&file_info.find_data_, sizeof(file_info.find_data_), 
             &find_data_, sizeof(find_data_));

    return file_info;
}

bool FileEnumerator::ShouldSkip(const FilePath& path)
{
    FilePath::PathString base_name = path.BaseName().value();
    if (base_name == FilePath::kCurrentDir || base_name == FilePath::kParentDir) {
        return true;
    }

    return false;
}

// --* FileInfo *--

FileEnumerator::FileInfo::FileInfo()
{
    memset(&find_data_, 0, sizeof(find_data_));
}

FileEnumerator::FileInfo::~FileInfo()
{}

bool FileEnumerator::FileInfo::IsDirectory() const
{
    return !!(find_data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

FilePath FileEnumerator::FileInfo::GetName() const
{
    return FilePath(find_data_.cFileName);
}

kbase::Time FileEnumerator::FileInfo::GetLastModifiedTime() const
{
    SYSTEMTIME last_modified_time;
    FileTimeToSystemTime(&find_data_.ftLastWriteTime, &last_modified_time);

    return last_modified_time;
}

uint64_t FileEnumerator::FileInfo::GetSize() const
{
    ULARGE_INTEGER file_size;
    file_size.LowPart = find_data_.nFileSizeLow;
    file_size.HighPart = find_data_.nFileSizeHigh;

    return file_size.QuadPart;
}

}   // namespace kbase