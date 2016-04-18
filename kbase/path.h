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

    // Checks whether `ch` is in kSeparators.
    static bool IsSeparator(value_type ch);

    // Returns true, if the path ends with a path separator.
    bool EndsWithSeparator() const;

    // Removes redundant trailing separator.
    // Note that, this function will not remove separator in path like: C:\,
    // because the separator here is not redundant.
    Path& StripTrailingSeparators();

    // Returns the path to the parent directory.
    // Returns empty path if the path itself is empty or there is only a single
    // element in the path.
    Path parent_path() const;

    // Returns a Path corresponding to the file component of the path. If this
    // object already refers to the root, returns the path directly.
    Path filename() const;

    // Retrieves every components of the path, including the root slash.
    // Example: C:\foo\bar  ->  ["C:", "\\", "foo", "bar"]
    void GetComponents(std::vector<string_type>& components) const;

    // Returns true if it is a absolute path.
    bool IsAbsolute() const;

    // |components| must be a relative path. Otherwise, functions will throw an
    // exception.

    Path& Append(const string_type& components);

    Path& Append(const Path& components);

    Path& AppendASCII(const std::string& components);

    Path AppendTo(const string_type& components) const;

    Path AppendTo(const Path& components) const;

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
    string_type extension() const;

    // Returns true, if `extension` matches the extension of the file name.
    bool MatchExtension(const string_type& extension) const;

    // Removes the extension of the path if there is any.
    Path& RemoveExtension();

    // Adds extension to the file name of the path.
    // If the file name of the path already has an extension, then the `extension` will
    // be the only extension recognized by Windows.
    Path& AddExtension(const string_type& extension);

    // Replaces the extension of the file name with `replacement`.
    // If `replacement` is empty or only contains separator, the extension of the file
    // name is removed.
    // If the file name does not have an extension, then `replacement` is added.
    Path& ReplaceExtension(const string_type& replacement);

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

    // Converts all directory separators in path to the preferred directory separator.
    Path& MakePreferredSeparator();

    Path& MakePathSeparatorTo(value_type separator);

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