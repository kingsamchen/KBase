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

    // Returns true, if the path ends with a path separator.
    bool EndsWithSeparator() const;

    // Returns a copy of the file path that ends with a path separator.
    // If the path is empty, returns an empty FilePath.
    FilePath AsEndingWithSeparator() const;

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

    // Returns the extension of the path if there is any.
    // The extension starts with extension separator.
    // If there are multiple extensions, Windows only recognizes the last one.
    PathString Extension() const;

    // Removes the extension of the path if there is any.
    void RemoveExtension();

    // Same as above, but strips on a copy and leaves the original path intact.
    FilePath StripExtention() const;

    // Inserts |suffix| after the file name portion of the path, but before the
    // extension.
    // Returns an empty FilePath if the BaseName() is '.' or '..'.
    // Example: path: c:\foo\bar\test.jpg suffix: (1) --> c:\foo\bar\test(1).jpg
    FilePath InsertBeforeExtension(const PathString& suffix) const;

    // Adds extension to the file name of the path.
    // If the file name of the path already has an extension, the |extension| will
    // be the sole extension recognized by Windows.
    // Returns an empty FilePath if the BaseName() is '.' or '..'.
    FilePath AddExtension(const PathString& extension) const;

    // Replaces the extension of the file name with |extension|.
    // If |extension| is empty or only contains separator, the extension of the file
    // name is removed.
    // If the file name does not have an extension, then |extension| is added.
    // Returns an empty FilePath if the BaseName() is '.' or '..'.
    FilePath ReplaceExtension(const PathString& extension) const;

    // Returns true, if |extension| matches the extension of the file name.
    bool MatchExtension(const PathString& extension) const;

    // Returns true if the path has a component that is '..'.
    bool ReferenceParent() const;

    // Case-insensitive comparison.
    // Returns -1, if str1 < str2;
    // Returns 0, if str1 == str2;
    // Returns 1, if str1 > str2.
    static int CompareIgnoreCase(const PathString& str1,
                                 const PathString& str2);

    static bool CompareEqualIgnoreCase(const PathString& str1,
                                       const PathString& str2)
    {
        return CompareIgnoreCase(str1, str2) == 0;
    }

private:
    void StripTrailingSeparatorsInternal();

private:
    PathString path_;
};

}   // namespace kbase

#endif  // KBASE_FILES_FILE_PATH_H_