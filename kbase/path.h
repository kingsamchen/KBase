/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILE_PATH_H_
#define KBASE_FILE_PATH_H_

#include <string>
#include <vector>

#include "kbase/basic_types.h"

namespace kbase {

class Path {
public:
    using value_type = PathChar;
    using string_type = std::basic_string<value_type>;

    static constexpr value_type kPreferredSeparator = L'\\';
    static constexpr const value_type kSeparators[] = L"\\/";
    static constexpr size_t kSeparatorCount = _countof(kSeparators) - 1;
    static constexpr value_type* kCurrentDir = L".";
    static constexpr value_type* kParentDir = L"..";
    static constexpr value_type kExtensionSeparator = L'.';
    static constexpr value_type kStringTerminator = L'\0';

    Path() = default;

    Path(const Path& other);

    Path(Path&& other);

    explicit Path(const string_type& path);

    Path& operator=(const Path& other);

    Path& operator=(Path&& other);

    ~Path() = default;

    // If two Path objects only differ in case, they are equal.
    friend bool operator==(const Path& lhs, const Path& rhs);

    friend bool operator!=(const Path& lhs, const Path& rhs);

    // Some STL contains require their elements have defined operator<.
    friend bool operator<(const Path& lhs, const Path& rhs);

    const string_type& value() const
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
    static bool IsSeparator(value_type ch);

    // Returns true, if the path ends with a path separator.
    bool EndsWithSeparator() const;

    // Returns a copy of the file path that ends with a path separator.
    // If the path is empty, returns an empty Path.
    Path AsEndingWithSeparator() const;

    // This function internally calls StripTrailingSeparatorInternal, which returns
    // the path that has been stripped.
    Path StripTrailingSeparators() const;

    // Returns a Path corresponding to the dir that contains the path. If this
    // object contains only one component, returns a Path identifying the current
    // dir. If this object already refers to the root dir, returns a Path
    // identifying the root dir.
    Path DirName() const;

    // Returns a Path corresponding to the file component of the path. If this
    // object already refers to the root, returns a Path identifying the root
    // dir.
    Path BaseName() const;

    // Retrieves every components of the path, including the root slash.
    // Example: C:\foo\bar  ->  ["C:", "\\", "foo", "bar"]
    void GetComponents(std::vector<string_type>* components) const;

    // Returns true if it is a absolute path.
    bool IsAbsolute() const;

    // |components| must be a relative path. Otherwise, functions will throw an
    // exception.

    void Append(const string_type& components);

    void Append(const Path& components);

    Path AppendTo(const string_type& components) const;

    Path AppendTo(const Path& components) const;

    void AppendASCII(const std::string& components);

    Path AppendASCIITo(const std::string& components) const;

    // If current path is parent of the |child|, appends to |path| the relative
    // path to child, and returns true.
    // otherwise, returns false.
    // Example: current path: C:\user\kingsley chen\
    //          child path:   C:\user\kingsley chen\app data\test
    //          *path:        C:\user\kingsley chen\documents\
    // After the calling of this function, *path becomes
    // C:\user\kingsley chen\documents\app data\test
    bool AppendRelativePath(const Path& child, Path* path) const;

    // Returns true, if the path is the parent of the |child|.
    // Returns false, otherwise.
    // NOTE: This function may make a wrong judgement if paths that contain both '.'
    // '..' are involved.
    bool IsParent(const Path& child) const;

    // Returns the extension of the path if there is any.
    // The extension starts with extension separator.
    // If there are multiple extensions, Windows only recognizes the last one.
    string_type Extension() const;

    // Removes the extension of the path if there is any.
    void RemoveExtension();

    // Same as above, but strips on a copy and leaves the original path intact.
    Path StripExtention() const;

    // Inserts |suffix| after the file name portion of the path, but before the
    // extension.
    // Returns an empty Path if the BaseName() is '.' or '..'.
    // Example: path: c:\foo\bar\test.jpg suffix: (1) --> c:\foo\bar\test(1).jpg
    Path InsertBeforeExtension(const string_type& suffix) const;

    // Adds extension to the file name of the path.
    // If the file name of the path already has an extension, the |extension| will
    // be the sole extension recognized by Windows.
    // Returns an empty Path if the BaseName() is '.' or '..'.
    Path AddExtension(const string_type& extension) const;

    // Replaces the extension of the file name with |extension|.
    // If |extension| is empty or only contains separator, the extension of the file
    // name is removed.
    // If the file name does not have an extension, then |extension| is added.
    // Returns an empty Path if the BaseName() is '.' or '..'.
    Path ReplaceExtension(const string_type& extension) const;

    // Returns true, if |extension| matches the extension of the file name.
    bool MatchExtension(const string_type& extension) const;

    // Returns true if the path has a component that is '..'.
    bool ReferenceParent() const;

    // Returns the string in native encoding of the path.
    // If the path contains any non-ASCII character, the return value is an empty
    // object.
    std::string AsASCII() const;

    std::string AsUTF8() const;

    // If the |path_in_ascii| contains any non-ASCII character, the function returns
    // an empty Path.
    static Path FromASCII(const std::string& path_in_ascii);

    static Path FromUTF8(const std::string& path_in_utf8);

    // We choose kSeparators[0] as our default path separator.
    Path NormalizePathSeparator() const;

    Path NormalizePathSeparatorTo(value_type separator) const;

    // Case-insensitive comparison.
    // Returns -1, if str1 < str2;
    // Returns 0, if str1 == str2;
    // Returns 1, if str1 > str2.
    static int CompareIgnoreCase(const string_type& str1,
                                 const string_type& str2);

    static bool CompareEqualIgnoreCase(const string_type& str1,
                                       const string_type& str2)
    {
        return CompareIgnoreCase(str1, str2) == 0;
    }

private:
    void StripTrailingSeparatorsInternal();

private:
    string_type path_;
};

}   // namespace kbase

namespace std {

template<>
struct std::hash<kbase::Path> {
    size_t operator()(const kbase::Path& file_path) const
    {
        return std::hash<kbase::Path::string_type>()(file_path.value());
    }
};

}   // namespace std

#endif  // KBASE_FILE_PATH_H_