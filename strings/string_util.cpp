// Author:  Kingsley Chen
// Date:    2014/02/11
// Purpose: core implementations of string util collection

#include "string_util.h"

#include <algorithm>
#include <cassert>
#include <functional>

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

enum TrimPosition {
    TRIM_NONE = 0,
    TRIM_LEADING = 0x1,
    TRIM_TAILING = 0x2,
    TRIM_ALL = TRIM_LEADING | TRIM_TAILING
};

template<typename strT>
TrimPosition TrimStringHelper(const strT& in, const typename strT::value_type trim_chars[],
                      TrimPosition pos, strT* out)
{
    typename strT::size_type last = in.length() - 1;
    typename strT::size_type not_matched_first = (pos & TrimPosition::TRIM_LEADING) ?
        in.find_first_not_of(trim_chars) : 0;
    typename strT::size_type not_matched_last = (pos & TrimPosition::TRIM_TAILING) ?
        in.find_last_not_of(trim_chars) : last;

    // in any case, we should clear |out|
    if (in.empty() || 
        not_matched_first == strT::npos || not_matched_last == strT::npos) {
        out->clear();
        return in.empty() ? TrimPosition::TRIM_NONE : pos;
    }

    *out = in.substr(not_matched_first, not_matched_last - not_matched_first + 1);
    
    unsigned leading_case = not_matched_first != 0 ?
        TrimPosition::TRIM_LEADING : TrimPosition::TRIM_NONE;
    unsigned tailing_case = not_matched_last != last ?
        TrimPosition::TRIM_TAILING : TrimPosition::TRIM_NONE;

    return static_cast<TrimPosition>(leading_case | tailing_case);
}

bool TrimString(const std::string& in, const char trim_chars[], std::string* out)
{
    return TrimStringHelper(in, trim_chars, TrimPosition::TRIM_ALL, out) !=
        TrimPosition::TRIM_NONE;
}

bool TrimString(const std::wstring& in, const wchar_t trim_chars[], std::wstring* out)
{
    return TrimStringHelper(in, trim_chars, TrimPosition::TRIM_ALL, out) !=
        TrimPosition::TRIM_NONE;
}

bool TrimLeadingStr(const std::string& in, const char trim_chars[], std::string* out)
{
    return TrimStringHelper(in, trim_chars, TrimPosition::TRIM_LEADING, out) ==
        TrimPosition::TRIM_LEADING;
}

bool TrimLeadingStr(const std::wstring& in, const wchar_t trim_chars[], std::wstring* out)
{
    return TrimStringHelper(in, trim_chars, TrimPosition::TRIM_LEADING, out) ==
        TrimPosition::TRIM_LEADING;
}

bool TrimTailingStr(const std::string& in, const char trim_chars[], std::string* out)
{
    return TrimStringHelper(in, trim_chars, TrimPosition::TRIM_TAILING, out) ==
        TrimPosition::TRIM_TAILING;
}

bool TrimTailingStr(const std::wstring& in, const wchar_t trim_chars[], std::wstring* out)
{
    return TrimStringHelper(in, trim_chars, TrimPosition::TRIM_TAILING, out) ==
        TrimPosition::TRIM_TAILING;
}

template<typename strT>
bool ContainsOnlyCharsT(const strT& in, const typename strT::value_type chars[])
{
    return in.find_first_not_of(chars, 0) == strT::npos;
}

bool ContainsOnlyChars(const std::string& in, const char chars[])
{
    return ContainsOnlyCharsT(in, chars);
}

bool ContainsOnlyChars(const std::wstring& in, const wchar_t chars[])
{
    return ContainsOnlyCharsT(in, chars);
}

template<typename strT>
bool StartsWithT(const strT& str, const strT& token, bool case_sensitive)
{
    if (str.length() < token.length())
        return false;

    if (case_sensitive) {
        return str.compare(0, token.length(), token) == 0;
    } else {
        auto icmp = [](typename strT::value_type lhs, typename strT::value_type rhs) -> bool {
            return ToLowerASCII(lhs) == ToLowerASCII(rhs);
        };

        return std::equal(token.cbegin(), token.cend(), str.cbegin(), icmp); 
    }
}

bool StartsWith(const std::string& str, const std::string& token, bool case_sensitive)
{
    return StartsWithT(str, token, case_sensitive);
}

bool StartsWith(const std::wstring& str, const std::wstring& token, bool case_sensitive)
{
    return StartsWithT(str, token, case_sensitive);
}

template<typename strT>
bool EndsWithT(const strT& str, const strT& token, bool case_sensitive)
{
    if (str.length() < token.length())
        return false;

    typename strT::size_type offset = str.length() - token.length();
    if (case_sensitive) {
        return str.compare(offset, token.length(), token)
            == 0;
    } else {
        auto icmp = [](typename strT::value_type lhs, typename strT::value_type rhs) -> bool {
            return ToLowerASCII(lhs) == ToLowerASCII(rhs);
        };

        return std::equal(token.cbegin(), token.cend(), str.cbegin() + offset, icmp);
    }
}

bool EndsWith(const std::string& str, const std::string& token, bool case_sensitive)
{
    return EndsWithT(str, token, case_sensitive);
}


bool EndsWith(const std::wstring& str, const std::wstring& token, bool case_sensitive)
{
    return EndsWithT(str, token, case_sensitive);
}

template<typename strT>
size_t TokenizeT(const strT& str, const strT& delimiters, std::vector<strT>* tokens)
{
    tokens->clear();

    typename strT::size_type begin = 0, end;
    while ((begin = str.find_first_not_of(delimiters, begin)) != strT::npos) {
        end = str.find_first_of(delimiters, begin);
        if (end == strT::npos)
            end = str.length();
        tokens->push_back(str.substr(begin, end - begin));
        begin = end + 1;
    }

    return tokens->size();
}

size_t Tokenize(const std::string& str, const std::string& delimiters,
                std::vector<std::string>* tokens)
{
    return TokenizeT(str, delimiters, tokens);
}

size_t Tokenize(const std::wstring& str, const std::wstring& delimiters,
                std::vector<std::wstring>* tokens)
{
    return TokenizeT(str, delimiters, tokens);
}


}   // namespace StringUtil

}   // namespace KBase