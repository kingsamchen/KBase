/*
 @ Kingsley Chen
*/

#include "kbase/files/file_path.h"

#include <cassert>
#include <cstdlib>

#include <algorithm>
#include <functional>
#include <stdexcept>

#include "kbase/strings/string_util.h"

namespace kbase {

typedef FilePath::PathChar PathChar;
typedef FilePath::PathString PathString;

const FilePath::PathChar FilePath::kSeparators[] = L"\\/";
const size_t FilePath::kSeparatorsLength = _countof(FilePath::kSeparators);
const FilePath::PathChar FilePath::kCurrentDir[] = L".";
const FilePath::PathChar FilePath::kParentDir[] = L"..";
const FilePath::PathChar FilePath::kExtensionSeparator = L'.';

namespace {

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

// This function adheres the equality stipulation for two FilePath objects:
// They can only differ in the case of drive letter.
bool EqualDriveLetterCaseInsensitive(const PathString& x, const PathString& y)
{
    auto letter_pos_x = FindDriveLetter(x);
    auto letter_pos_y = FindDriveLetter(y);

    // if only one contains a drive letter, the comparison result is same as
    // x == y
    if (letter_pos_x == PathString::npos || letter_pos_y == PathString::npos) {
        return x == y;
    }

    PathString&& letter_x = x.substr(0, letter_pos_x + 1);
    PathString&& letter_y = y.substr(0, letter_pos_y + 1);
    if (!kbase::StartsWith(letter_x, letter_y, false)) {
        return false;
    }

    PathString&& rest_x = x.substr(letter_pos_x + 1);
    PathString&& rest_y = y.substr(letter_pos_y + 1);
    return rest_x == rest_y;
}

}   // namespace

FilePath::FilePath()
{}

FilePath::FilePath(const FilePath& other)
 : path_(other.path_)
{}

FilePath::FilePath(const PathString& path)
 : path_(path)
{
    //TODO: what if path has someting after \0 ?
}

FilePath& FilePath::operator=(const FilePath& other)
{
    path_ = other.path_;
    return *this;
}

FilePath::~FilePath()
{}

bool operator==(const FilePath& lhs, const FilePath& rhs)
{
    return EqualDriveLetterCaseInsensitive(lhs.value(), rhs.value());    
}

bool operator!=(const FilePath& lhs, const FilePath& rhs)
{
    return !EqualDriveLetterCaseInsensitive(lhs.value(), rhs.value());
}

// static
bool FilePath::IsSeparator(PathChar ch)
{
    using std::placeholders::_1;
    using std::equal_to;

    return std::any_of(std::begin(kSeparators), std::end(kSeparators),
                       std::bind(equal_to<PathChar>(), ch, _1));
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
    if (!components) {
        throw std::invalid_argument("components cannot be NULL!");
    }

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

}   // namespace kbase