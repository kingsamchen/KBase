// Author:  Kingsley Chen
// Date:    2014/01/27
// Purpose: core implementations of string util collection

#include "string_util.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <string>

#include "string_piece.h"

namespace KBase {

namespace StringUtil {

template<typename strT>
bool RemoveCharsT(const strT& in, const KBase::BasicStringPiece<strT>& remove_chars, strT* out)
{
    strT tmp(in.size(), 0);

    auto char_in = [=](typename strT::value_type ch) -> bool {
        return std::find(remove_chars.cbegin(), remove_chars.cend(), ch) 
            != remove_chars.cend();
    };

    auto new_end = std::remove_copy_if(in.cbegin(), in.cend(), tmp.begin(), char_in);
    tmp.erase(new_end, tmp.end());
    using std::swap;
    swap(tmp, *out);

    return out->size() != in.size();
}

bool RemoveChars(const std::string& in, const char remove_chars[], std::string* out)
{
    return RemoveCharsT<std::string>(in, remove_chars, out);
}

bool RemoveChars(const std::wstring& in, const wchar_t remove_chars[], std::wstring* out)
{
    return RemoveCharsT<std::wstring>(in, remove_chars, out);
}

template<typename strT>
void ReplaceSubstrHelper(strT* str, const strT& find_with, const strT& replace_with,
                         typename strT::size_type pos, bool replace_all)
{
    if (pos == strT::npos || pos + find_with.length() > str->length()) {
        return;
    }

    assert(str);
    assert(!find_with.empty());

    typename strT::size_type offset = pos;
    while ((offset = str->find(find_with, offset)) != strT::npos) {
        str->replace(offset, find_with.length(), replace_with);
        offset += replace_with.length();

        if (!replace_all) break;
    }
}

void ReplaceSubstr(std::string* str, const std::string& find_with,
                   const std::string& replace_with, std::string::size_type pos)
{
    ReplaceSubstrHelper(str, find_with, replace_with, pos, true);
}

void ReplaceSubstr(std::wstring* str, const std::wstring& find_with,
                   const std::wstring& replace_with, std::wstring::size_type pos)
{
    ReplaceSubstrHelper(str, find_with, replace_with, pos, true);
}

void ReplaceFirstSubstr(std::string* str, const std::string& find_with,
                        const std::string& replace_with, std::string::size_type pos)
{
    ReplaceSubstrHelper(str, find_with, replace_with, pos, false);
}

void ReplaceFirstSubstr(std::wstring* str, const std::wstring& find_with,
                        const std::wstring& replace_with, std::wstring::size_type pos)
{
    ReplaceSubstrHelper(str, find_with, replace_with, pos, false);
}

}   // namespace StringUtil

}   // namespace KBase