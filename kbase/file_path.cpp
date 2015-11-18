/*
 @ 0xCCCCCCCC
*/

#include "kbase\file_path.h"

#include <cassert>
#include <cstdlib>

#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>

#include "kbase\error_exception_util.h"
#include "kbase\pickle.h"
#include "kbase\string_util.h"
#include "kbase\sys_string_encoding_conversions.h"

namespace kbase {

const PathChar FilePath::kSeparators[] = L"\\/";
const size_t FilePath::kSeparatorsLength = _countof(FilePath::kSeparators);
const PathChar FilePath::kCurrentDir[] = L".";
const PathChar FilePath::kParentDir[] = L"..";
const PathChar FilePath::kExtensionSeparator = L'.';
const PathChar FilePath::kStringTerminator = L'\0';

}   // namespace kbase

namespace {

using kbase::FilePath;

typedef kbase::PathChar PathChar;
typedef kbase::PathString PathString;

// If the |path| contains a drive letter specification, returns the position of the
// last character of the specification; otherwise, return npos.
PathString::size_type FindDriveLetter(const PathString& path)
{
    if (path.length() >= 2 && path[1] == L':' &&
        ((path[0] >= L'A' && path[0] <= L'Z') ||
         (path[0] >= L'a' && path[0] <= L'z'))) {
        return 1;
    }

    return PathString::npos;
}

inline bool EqualPathString(const PathString& x, const PathString& y)
{
    return !kbase::SysStringCompareCaseInsensitive(x, y);
}

// This function adheres the equality stipulated for two FilePath objects:
// They can differ in the case, which is permitted by Windows.
bool EqualDriveLetterCaseInsensitive(const PathString& x, const PathString& y)
{
    auto letter_pos_x = FindDriveLetter(x);
    auto letter_pos_y = FindDriveLetter(y);

    // if only one contains a drive letter, the comparison result is same as
    // x == y
    if (letter_pos_x == PathString::npos || letter_pos_y == PathString::npos) {
        return EqualPathString(x, y);
    }

    PathString&& letter_x = x.substr(0, letter_pos_x + 1);
    PathString&& letter_y = y.substr(0, letter_pos_y + 1);
    if (!EqualPathString(letter_x, letter_y)) {
        return false;
    }

    PathString&& rest_x = x.substr(letter_pos_x + 1);
    PathString&& rest_y = y.substr(letter_pos_y + 1);
    return EqualPathString(rest_x, rest_y);
}

bool IsPathAbsolute(const PathString& path)
{
    PathString::size_type drive_letter = FindDriveLetter(path);

    // Such as c:\foo or \\foo .etc
    if (drive_letter != PathString::npos) {
        return (drive_letter + 1 < path.length()) &&
            (FilePath::IsSeparator(path[drive_letter + 1]));
    }

    return (path.length() > 1) &&
        (FilePath::IsSeparator(path[0]) && FilePath::IsSeparator(path[1]));
}

PathString::size_type ExtensionSeparatorPosition(const PathString& path)
{
    // Special cases for which path contains '.' but does not follow with an extension.
    if (path == FilePath::kCurrentDir || path == FilePath::kParentDir) {
        return PathString::npos;
    }

    return path.rfind(FilePath::kExtensionSeparator);
}

bool IsPathEmptyOrSpecialCase(const PathString& path)
{
    if (path.empty() || path == FilePath::kCurrentDir ||
        path == FilePath::kParentDir) {
        return true;
    }

    return false;
}

}   // namespace

namespace kbase {

FilePath::FilePath(const FilePath& other)
    : path_(other.path_)
{}

FilePath::FilePath(FilePath&& other)
    : path_(std::move(other.path_))
{}

FilePath::FilePath(const PathString& path)
    : path_(path)
{
    // The null-terminator '\0' indicates the end of a path.
    PathString::size_type null_pos = path_.find(kStringTerminator);
    if (null_pos != PathString::npos) {
        path_ = path_.substr(0, null_pos);
    }
}

FilePath& FilePath::operator=(const FilePath& other)
{
    path_ = other.path_;
    return *this;
}

FilePath& FilePath::operator=(FilePath&& other)
{
    path_ = std::move(other.path_);
    return *this;
}

bool operator==(const FilePath& lhs, const FilePath& rhs)
{
    return EqualDriveLetterCaseInsensitive(lhs.value(), rhs.value());
}

bool operator!=(const FilePath& lhs, const FilePath& rhs)
{
    return !EqualDriveLetterCaseInsensitive(lhs.value(), rhs.value());
}

bool operator<(const FilePath& lhs, const FilePath& rhs)
{
    return (kbase::SysStringCompareCaseInsensitive(lhs.value(), rhs.value()) < 0);
}

// static
bool FilePath::IsSeparator(PathChar ch)
{
    using std::placeholders::_1;
    using std::equal_to;

    return std::any_of(std::begin(kSeparators), std::end(kSeparators),
                       std::bind(equal_to<PathChar>(), ch, _1));
}

bool FilePath::EndsWithSeparator() const
{
    if (empty()) {
        return false;
    }

    return IsSeparator(path_.back());
}

FilePath FilePath::AsEndingWithSeparator() const
{
    if (empty() || EndsWithSeparator()) {
        return FilePath(*this);
    }

    PathString new_path_str;
    new_path_str.reserve(path_.length() + 1);

    new_path_str = path_;
    new_path_str.append(1, kSeparators[0]);

    return FilePath(new_path_str);
}

void FilePath::StripTrailingSeparatorsInternal()
{
    // start always points to the position one-offset past the leading separator
    PathString::size_type start = FindDriveLetter(path_) + 2;

    PathString::size_type last_stripped = PathString::npos;
    PathString::size_type pos = path_.length();
    for (; pos > start && IsSeparator(path_[pos-1]); --pos) {
        if (pos != start + 1 ||
            !IsSeparator(path_[start-1]) ||
            last_stripped != PathString::npos) {
            last_stripped = pos;
        } else {
            break;
        }
    }

    path_.resize(pos);
}

FilePath FilePath::StripTrailingSeparators() const
{
    FilePath new_path(path_);
    new_path.StripTrailingSeparatorsInternal();

    return new_path;
}

FilePath FilePath::DirName() const
{
    FilePath new_path(path_);
    new_path.StripTrailingSeparatorsInternal();

    auto letter = FindDriveLetter(new_path.path_);
    auto last_separator = new_path.path_.find_last_of(kSeparators, PathString::npos,
                                                      kSeparatorsLength - 1);

    // there might be a drive letter in the path
    if (last_separator == PathString::npos) {
        // in current dir
        new_path.path_.resize(letter + 1);
    } else if (last_separator == letter + 1) {
        // in root dir
        new_path.path_.resize(letter + 2);
    } else if (last_separator == letter + 2 &&
               IsSeparator(new_path.path_[letter+1])) {
        // preserves the leading double-separator
        new_path.path_.resize(letter + 3);
    } else {
        new_path.path_.resize(last_separator);
    }

    new_path.StripTrailingSeparatorsInternal();
    if (new_path.path_.empty()) {
        new_path.path_ = kCurrentDir;
    }

    return new_path;
}

FilePath FilePath::BaseName() const
{
    FilePath new_path(path_);
    new_path.StripTrailingSeparatorsInternal();

    auto letter = FindDriveLetter(new_path.path_);
    if (letter != PathString::npos) {
        new_path.path_.erase(0, letter + 1);
    }

    auto last_separator = new_path.path_.find_last_of(kSeparators, PathString::npos,
                                                      kSeparatorsLength - 1);
    if (last_separator != PathString::npos &&
        last_separator < new_path.path_.length() - 1) {
        new_path.path_.erase(0, last_separator + 1);
    }

    return new_path;
}

void FilePath::GetComponents(std::vector<PathString>* components) const
{
    assert(components);
#if defined(NDEBUG)
    if (!components) {
        throw std::invalid_argument("components cannot be NULL!");
    }
#endif

    components->clear();

    if (path_.empty()) {
        return;
    }

    std::vector<PathString> parts;
    FilePath current(path_);
    FilePath base;

    auto AreAllSeparators = [](const PathString& path) -> bool {
        return std::all_of(path.cbegin(), path.cend(), FilePath::IsSeparator);
    };

    // main body
    while (current != current.DirName()) {
        base = current.BaseName();
        if (!AreAllSeparators(base.value())) {
            parts.push_back(base.value());
        }
        current = current.DirName();
    }

    // root
    base = current.BaseName();
    if (!base.empty() && base.value() != kCurrentDir) {
        parts.push_back(base.value());
    }

    // drive letter
    auto letter = FindDriveLetter(current.value());
    if (letter != PathString::npos) {
        parts.push_back(current.value().substr(0, letter + 1));
    }

    std::copy(parts.crbegin(), parts.crend(), std::back_inserter(*components));
}

bool FilePath::IsAbsolute() const
{
    return IsPathAbsolute(path_);
}

void FilePath::Append(const PathString& components)
{
    const PathString* need_appended = &components;
    PathString without_null;

    PathString::size_type null_pos = need_appended->find(kStringTerminator);
    if (null_pos != PathString::npos) {
        without_null = components.substr(0, null_pos);
        need_appended = &without_null;
    }

    ENSURE(!IsPathAbsolute(*need_appended)).raise();

    // If appends to the current dir, just set the path as the components.
    if (path_ == kCurrentDir) {
        path_ = *need_appended;
        return;
    }

    StripTrailingSeparatorsInternal();

    // If the path is empty, that indicates current directory.
    // If the path component is empty, that indicates nothing to append.
    if (!need_appended->empty() && !path_.empty()) {
        // Don't append separator, if there is already one.
        if (!IsSeparator(path_[path_.length()-1])) {
            // Don't append separator, if the path is a drive letter,
            // which is a valid relative path.
            if (FindDriveLetter(path_) + 1 != path_.length()) {
                path_.append(1, kSeparators[0]);
            }
        }
    }

    path_.append(*need_appended);
}

void FilePath::Append(const FilePath& components)
{
    Append(components.value());
}

FilePath FilePath::AppendTo(const PathString& components) const
{
    FilePath new_path(*this);
    new_path.Append(components);

    return new_path;
}

FilePath FilePath::AppendTo(const FilePath& components) const
{
    return AppendTo(components.value());
}

bool FilePath::AppendRelativePath(const FilePath& child, FilePath* path) const
{
    std::vector<PathString> current_components;
    std::vector<PathString> child_components;
    GetComponents(&current_components);
    child.GetComponents(&child_components);

    if (current_components.empty() ||
        current_components.size() >= child_components.size()) {
        return false;
    }

    auto current_iter = current_components.cbegin();
    auto child_iter = child_components.cbegin();
    for (; current_iter != current_components.cend(); ++current_iter, ++child_iter) {
        if (!EqualPathString(*current_iter, *child_iter)) {
            return false;
        }
    }

    if (path) {
        for (; child_iter != child_components.cend(); ++child_iter) {
            path->Append(*child_iter);
        }
    }

    return true;
}

bool FilePath::IsParent(const FilePath& child) const
{
    return AppendRelativePath(child, nullptr);
}

void FilePath::AppendASCII(const std::string& components)
{
    ENSURE(IsStringASCII(components)).raise();

    Append(ASCIIToWide(components));
}

kbase::FilePath FilePath::AppendASCIITo(const std::string& components) const
{
    FilePath new_path(*this);
    new_path.AppendASCII(components);

    return new_path;
}

PathString FilePath::Extension() const
{
    FilePath base(BaseName());
    PathString::size_type separator_pos = ExtensionSeparatorPosition(base.path_);

    if (separator_pos == PathString::npos) {
        return PathString();
    }

    return base.path_.substr(separator_pos);
}

void FilePath::RemoveExtension()
{
    if (Extension().empty()) {
        return;
    }

    PathString::size_type separator_pos = ExtensionSeparatorPosition(path_);
    if (separator_pos != PathString::npos) {
        path_ = path_.substr(0, separator_pos);
    }
}

FilePath FilePath::StripExtention() const
{
    FilePath new_path(path_);
    new_path.RemoveExtension();

    return new_path;
}

FilePath FilePath::InsertBeforeExtension(const PathString& suffix) const
{
    if (suffix.empty()) {
        return FilePath(*this);
    }

    if (IsPathEmptyOrSpecialCase(BaseName().value())) {
        return FilePath();
    }

    PathString extension = Extension();
    FilePath new_path = StripExtention();
    new_path.path_.append(suffix).append(extension);

    return new_path;
}

FilePath FilePath::AddExtension(const PathString& extension) const
{
    if (IsPathEmptyOrSpecialCase(BaseName().value())) {
        return FilePath();
    }

    if (extension.empty() || extension == PathString(1, kExtensionSeparator)) {
        return FilePath(*this);
    }

    FilePath new_path(path_);

    // If neither the path nor the extension contains the separator, adds
    // one manually.
    if (new_path.path_.back() != kExtensionSeparator &&
        extension.front() != kExtensionSeparator) {
        new_path.path_.append(1, kExtensionSeparator);
    }

    new_path.path_.append(extension);

    return new_path;
}

FilePath FilePath::ReplaceExtension(const PathString& extension) const
{
    if (IsPathEmptyOrSpecialCase(BaseName().value())) {
        return FilePath();
    }

    FilePath new_path = StripExtention();

    if (extension.empty() || extension == PathString(1, kExtensionSeparator)) {
        return new_path;
    }

    if (extension[0] != kExtensionSeparator) {
        new_path.path_.append(1, kExtensionSeparator);
    }

    new_path.path_.append(extension);

    return new_path;
}

bool FilePath::MatchExtension(const PathString& extension) const
{
    PathString ext = Extension();
    return kbase::StringToLowerASCII(ext) == kbase::StringToLowerASCII(extension);
}

bool FilePath::ReferenceParent() const
{
    std::vector<PathString> components;
    GetComponents(&components);

    // It seems redundant spaces at the tail of '..' can be ignored by Windows.
    PathString trimed_component;
    for (const PathString& component : components) {
        kbase::TrimTailingStr(component, L" ", &trimed_component);
        if (trimed_component == kParentDir) {
            return true;
        }
    }

    return false;
}

std::string FilePath::AsASCII() const
{
    if (!kbase::IsStringASCII(path_)) {
        return std::string();
    }

    return kbase::WideToASCII(path_);
}

std::string FilePath::AsUTF8() const
{
    return kbase::SysWideToUTF8(path_);
}

// static
FilePath FilePath::FromASCII(const std::string& path_in_ascii)
{
    if (!kbase::IsStringASCII(path_in_ascii)) {
        return FilePath();
    }

    return FilePath(kbase::ASCIIToWide(path_in_ascii));
}

// static
FilePath FilePath::FromUTF8(const std::string& path_in_utf8)
{
    return FilePath(kbase::SysUTF8ToWide(path_in_utf8));
}

void FilePath::WriteToPickle(Pickle* pickle) const
{
    pickle->Write(path_);
}

bool FilePath::ReadFromPickle(PickleIterator* iter)
{
    if (!iter->Read(&path_)) {
        return false;
    }

    if (path_.find(kStringTerminator) != PathString::npos) {
        return false;
    }

    return true;
}

FilePath FilePath::NormalizePathSeparatorTo(PathChar separator) const
{
    PathString new_path_str = path_;

    for (PathChar sep : kSeparators) {
        std::replace(new_path_str.begin(), new_path_str.end(), sep, separator);
    }

    return FilePath(new_path_str);
}

FilePath FilePath::NormalizePathSeparator() const
{
    return NormalizePathSeparatorTo(kSeparators[0]);
}

// static
int FilePath::CompareIgnoreCase(const PathString& str1, const PathString& str2)
{
    return kbase::SysStringCompareCaseInsensitive(str1, str2);
}

}   // namespace kbase