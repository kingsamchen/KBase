/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILE_INFO_H_
#define KBASE_FILE_INFO_H_

#include <cstdint>

#include "kbase/basic_types.h"
#include "kbase/chrono_util.h"

namespace kbase {

class FileTime {
public:
    using time_type = TimePoint;

    explicit FileTime(time_type time)
        : time_(time)
    {}

#if defined(OS_WIN)
    explicit FileTime(const FILETIME& winft)
        : time_(TimePointFromWindowsFileTime(winft))
    {}

    explicit FileTime(const SYSTEMTIME& winst)
        : time_(TimePointFromWindowsSystemTime(winst))
    {}
#endif

    time_type value() const
    {
        return time_;
    }

private:
    time_type time_;
};

class FileInfo {
public:
    FileInfo(const PathString& name,
             int64_t size,
             bool is_directory,
             const FileTime& creation_time,
             const FileTime& last_modified_time,
             const FileTime& last_accessed_time)
        : name_(name),
          size_(size),
          is_directory_(is_directory),
          creation_time_(creation_time),
          last_modified_time_(last_modified_time),
          last_accessed_time_(last_accessed_time)
    {}

    ~FileInfo() = default;

    const PathString& name() const
    {
        return name_;
    }

    int64_t size() const
    {
        return size_;
    }

    bool is_directory() const
    {
        return is_directory_;
    }

    FileTime creation_time() const
    {
        return creation_time_;
    }

    FileTime last_modified_time() const
    {
        return last_modified_time_;
    }

    FileTime last_accessed_time() const
    {
        return last_accessed_time_;
    }

private:
    PathString name_;
    int64_t size_;
    bool is_directory_;
    FileTime creation_time_;
    FileTime last_modified_time_;
    FileTime last_accessed_time_;
};

}   // namespace kbase

#endif  // KBASE_FILE_INFO_H_