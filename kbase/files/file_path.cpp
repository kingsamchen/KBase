/*
 @ Kingsley Chen
*/

#include "kbase/files/file_path.h"

#include <cassert>
#include <cstdlib>

#include <algorithm>
#include <functional>
#include <stdexcept>

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
    // start always points to the position one-offset past the leading slash
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

    if (last_separator == PathString::npos) {
        new_path.path_.resize(letter + 1);
    } else if (last_separator == letter + 1) {
        new_path.path_.resize(letter + 2);
    } else if (last_separator == letter + 2 &&
               IsSeparator(new_path.path_[letter+1])) {
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

    //size_t slash_pos = path_.find_first_of(kSeparators);
    //if (slash_pos == 0) {
    //    components->emplace_back(1, path_[slash_pos]);
    //} else if (path_[slash_pos-1] == L':') {
    //    components->emplace_back(path_.substr(0, slash_pos));
    //    components->emplace_back(1, path_[slash_pos]);
    //}

    //std::vector<std::wstring> parts;
    //kbase::Tokenize(path_.substr(slash_pos + 1), kSeparators, &parts);

    //std::remove_copy(parts.cbegin(), parts.cend(), std::back_inserter(*components),
    //                 kCurrentDir);
}

}   // namespace kbase