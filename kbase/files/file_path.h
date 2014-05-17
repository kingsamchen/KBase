/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILES_FILE_PATH_H_
#define KBASE_FILES_FILE_PATH_H_

#include <string>
#include <vector>

#include "kbase/strings/string_piece.h"

namespace kbase {

class FilePath {
public:
    typedef std::wstring PathString;
    typedef PathString::value_type PathChar;

    // Separators in path hierarchy. supports both '/' and '\'.
    static const PathChar kSeparators[];
    static const size_t kSeparatorsLength;

    static const PathChar kCurrentDir[];
    static const PathChar kParentDir[];
    static const PathChar kExtensionSeparator;
    static const PathChar kStringTerminator;

    FilePath();
    
    FilePath(const FilePath& other);

    explicit FilePath(const PathString& path);

    FilePath& operator=(const FilePath& other);

    ~FilePath();

    // If two FilePath objects only differ in case, they are equal.
    friend bool operator==(const FilePath& lhs, const FilePath& rhs);

    friend bool operator!=(const FilePath& lhs, const FilePath& rhs);

    // Some STL contains require their elements have defined operator<.
    friend bool operator<(const FilePath& lhs, const FilePath& rhs);

    const PathString& value() const
    {
        return path_;
    }

    bool empty() const
    {
        return path_.empty();
    }

    void clear()
    {
        path_.clear();
    }

    // Checks whether |ch| is in kSeparators.
    static bool IsSeparator(PathChar ch);

    // This function internally calls StripTrailingSeparatorInternal, which returns
    // the path that has been stripped.
    FilePath StripTrailingSeparators() const;

    // Returns a FilePath corresponding to the dir that contains the path. If this
    // object contains only one component, returns a FilePath identifying the current
    // dir. If this object already refers to the root dir, returns a FilePath
    // identifying the root dir.
    FilePath DirName() const;

    // Returns a FilePath corresponding to the file component of the path. If this
    // object already refers to the root, returns a FilePath identifying the root
    // dir.
    FilePath BaseName() const;

    // Retrieves every components of the path, including the root slash.
    // Example: C:\foo\bar  ->  ["C:", "\\", "foo", "bar"]
    void GetComponents(std::vector<PathString>* components) const;
    
    // Returns true
    bool IsAbsolute() const;

    // |components| must be a relative path. Otherwise, functions will throw an
    // invalid_argument exception.
    void Append(const PathString& components);
    void Append(const FilePath& components);

    void AppendASCII(const StringPiece& components);

    // If current path is parent of the |child|, appends to |path| the relative
    // path to child, and returns true.
    // otherwise, returns false.
    // Example: current path: C:\user\kingsley chen\
    //          child path:   C:\user\kingsley chen\app data\test
    //          *path:        C:\user\kingsley chen\documents\
    // After the calling of this function, *path becomes
    // C:\user\kingsley chen\documents\app data\test
    bool AppendRelativePath(const FilePath& child, FilePath* path) const;

    // Returns true, if the path is the parent of the |child|.
    // Returns false, otherwise.
    bool IsParent(const FilePath& child) const;

private:
    void StripTrailingSeparatorsInternal();

private:
    PathString path_;
};

}   // namespace kbase

#endif  // KBASE_FILES_FILE_PATH_H_