
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILES_FILE_INFO_H_
#define KBASE_FILES_FILE_INFO_H_

#include <cstdint>

#include "kbase\basic_types.h"
#include "kbase\date_time.h"

namespace kbase {

class FileInfo {
public:
    FileInfo(const PathString& name,
             int64_t size,
             bool is_directory,
             const DateTime& creation_time,
             const DateTime& last_modified_time,
             const DateTime& last_accessed_time)
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

    DateTime creation_time() const
    {
        return creation_time_;
    }

    DateTime last_modified_time() const
    {
        return last_modified_time_;
    }

    DateTime last_accessed_time() const
    {
        return last_accessed_time_;
    }

private:
    PathString name_;
    int64_t size_;
    bool is_directory_;
    DateTime creation_time_;
    DateTime last_modified_time_;
    DateTime last_accessed_time_;
};

}   // namespace kbase

#endif  // KBASE_FILES_FILE_INFO_H_