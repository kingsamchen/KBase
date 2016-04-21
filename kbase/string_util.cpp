/*
 @ 0xCCCCCCCC
*/

#include "kbase/string_util.h"

#include <Windows.h>

#include <algorithm>
#include <cassert>
#include <cctype>

#include "kbase/error_exception_util.h"

namespace {

using kbase::BasicStringPiece;
using kbase::ToUnsigned;

inline int ToLower(char ch)
{
    return tolower(ch);
}

inline int ToLower(wchar_t ch)
{
    return towlower(ch);
}

inline int ToUpper(char ch)
{
    return toupper(ch);
}

inline int ToUpper(wchar_t ch)
{
    return towupper(ch);
}

enum TrimPosition : unsigned int {
    TRIM_NONE = 0,
    TRIM_LEADING = 1 << 0,
    TRIM_TAILING = 1 << 1,
    TRIM_ALL = TRIM_LEADING | TRIM_TAILING
};

template<typename strT>
void RemoveCharsT(strT& str, BasicStringPiece<strT> remove_chars)
{
    auto char_in = [remove_chars](typename strT::value_type ch) -> bool {
        return std::find(remove_chars.cbegin(), remove_chars.cend(), ch) != remove_chars.cend();
    };

    auto new_end = std::remove_if(str.begin(), str.end(), char_in);
    str.erase(new_end, str.end());
}

template<typename strT>
void ReplaceSubstringT(strT& str,
                       BasicStringPiece<strT> find_with,
                       BasicStringPiece<strT> replace_with,
                       typename strT::size_type pos,
                       bool replace_all)
{
    if (pos == strT::npos || pos + find_with.length() > str.length()) {
        return;
    }

    typename strT::size_type offset = pos;
    while ((offset = str.find(find_with.data(), offset, find_with.length())) != strT::npos) {
        str.replace(offset, find_with.length(), replace_with.data(), replace_with.length());
        offset += replace_with.length();
        if (!replace_all) {
            break;
        }
    }
}

template<typename strT>
void TrimStringT(strT& str, BasicStringPiece<strT> trim_chars, TrimPosition pos)
{
    using size_type = typename strT::size_type;

    size_type last = str.length() - 1;
    size_type not_matched_first = (pos & TrimPosition::TRIM_LEADING) ?
        str.find_first_not_of(trim_chars.data(), 0, trim_chars.length()) : 0;
    size_type not_matched_last = (pos & TrimPosition::TRIM_TAILING) ?
        str.find_last_not_of(trim_chars.data(), strT::npos, trim_chars.size()) : last;

    if (str.empty() || not_matched_first == strT::npos || not_matched_last == strT::npos) {
        str.clear();
        return;
    }

    str = str.substr(not_matched_first, not_matched_last - not_matched_first + 1);
}

template<typename strT>
bool ContainsOnlyCharsT(const strT& str, BasicStringPiece<strT> chars)
{
    return str.find_first_not_of(chars.data(), 0, chars.size()) == strT::npos;
}

template<typename strT>
bool StartsWithT(const strT& str, BasicStringPiece<strT> token, bool case_sensitive)
{
    if (str.length() < token.length()) {
        return false;
    }

    if (case_sensitive) {
        return str.compare(0, token.length(), token.data()) == 0;
    }

    auto icmp = [](typename strT::value_type lhs, typename strT::value_type rhs) {
        return ToLower(lhs) == ToLower(rhs);
    };

    return std::equal(token.cbegin(), token.cend(), str.cbegin(), icmp);
}

template<typename strT>
bool EndsWithT(const strT& str, BasicStringPiece<strT> token, bool case_sensitive)
{
    if (str.length() < token.length()) {
        return false;
    }

    typename strT::size_type offset = str.length() - token.length();
    if (case_sensitive) {
        return str.compare(offset, token.length(), token.data()) == 0;
    }

    auto icmp = [](typename strT::value_type lhs, typename strT::value_type rhs) {
        return ToLower(lhs) == ToLower(rhs);
    };

    return std::equal(token.cbegin(), token.cend(), str.cbegin() + offset, icmp);
}

template<typename strT>
size_t SplitStringT(const strT& str, BasicStringPiece<strT>delimiters, std::vector<strT>& tokens)
{
    tokens.clear();

    typename strT::size_type begin = 0, end;
    while ((begin = str.find_first_not_of(delimiters.data(), begin, delimiters.length()))
           != strT::npos) {
        end = str.find_first_of(delimiters.data(), begin, delimiters.length());
        if (end == strT::npos) {
            end = str.length();
        }
        tokens.push_back(str.substr(begin, end - begin));
        begin = end + 1;
    }

    return tokens.size();
}

template<typename strT>
strT JoinStringT(const std::vector<strT>& tokens, BasicStringPiece<strT> sep)
{
    if (tokens.empty()) {
        return strT();
    }

    strT str(tokens[0]);
    for (auto it = tokens.cbegin() + 1; it != tokens.cend(); ++it) {
        str.append(sep.data(), sep.length()).append(*it);
    }

    return str;
}

template<typename charT>
bool MatchPatternT(const charT* str, const charT* pat)
{
    bool on_star = false;
    const charT* s;
    const charT* p;

LoopStart:
    for (s = str, p = pat; *s; ++s, ++p) {
        switch (*p) {
            case '?':
                if (*s == '.')
                    goto StarCheck;
                break;
            case '*':
                on_star = true;
                str = s, pat = p;
                do {
                    ++pat;
                } while (*pat == '*');
                if (!*pat) return true;
                goto LoopStart;
                break;
            default:
                if (*s != *p)
                    goto StarCheck;
                break;
        }
    }

    while (*p == '*')
        ++p;

    return !*p;

StarCheck:
    if (!on_star) return false;
    ++str;
    goto LoopStart;
}

template<typename viewT>
bool StringASCIICheck(viewT str)
{
    for (size_t i = 0; i < str.length(); ++i) {
        typename ToUnsigned<typename viewT::value_type>::Unsigned c = str[i];
        if (c > 0x7F) {
            return false;
        }
    }

    return true;
}

}   // namespace

namespace kbase {

void RemoveChars(std::string& str, StringPiece remove_chars)
{
    RemoveCharsT(str, remove_chars);
}

void RemoveChars(std::wstring& str, WStringPiece remove_chars)
{
    RemoveCharsT(str, remove_chars);
}

void ReplaceSubstring(std::string& str,
                      StringPiece find_with,
                      StringPiece replace_with,
                      std::string::size_type pos,
                      bool replace_all)
{
    ReplaceSubstringT(str, find_with, replace_with, pos, replace_all);
}

void ReplaceSubstring(std::wstring& str,
                      WStringPiece find_with,
                      WStringPiece replace_with,
                      std::wstring::size_type pos,
                      bool replace_all)
{
    ReplaceSubstringT(str, find_with, replace_with, pos, replace_all);
}

void TrimString(std::string& str, StringPiece trim_chars)
{
    TrimStringT(str, trim_chars, TrimPosition::TRIM_ALL);
}

void TrimString(std::wstring& str, WStringPiece trim_chars)
{
    TrimStringT(str, trim_chars, TrimPosition::TRIM_ALL);
}

void TrimLeadingString(std::string& str, StringPiece trim_chars)
{
    TrimStringT(str, trim_chars, TrimPosition::TRIM_LEADING);
}

void TrimLeadingString(std::wstring& str, WStringPiece trim_chars)
{
    TrimStringT(str, trim_chars, TrimPosition::TRIM_LEADING);
}

void TrimTailingString(std::string& str, StringPiece trim_chars)
{
    TrimStringT(str, trim_chars, TrimPosition::TRIM_TAILING);
}

void TrimTailingString(std::wstring& str, WStringPiece trim_chars)
{
    TrimStringT(str, trim_chars, TrimPosition::TRIM_TAILING);
}

bool ContainsOnlyChars(const std::string& str, StringPiece chars)
{
    return ContainsOnlyCharsT(str, chars);
}

bool ContainsOnlyChars(const std::wstring& str, WStringPiece chars)
{
    return ContainsOnlyCharsT(str, chars);
}

void StringToLower(std::string& str)
{
    if (str.empty()) {
        return;
    }

#if !defined(_M_X64)
    auto buff = WriteInto(str, str.length() + 1);
    CharLowerBuffA(buff, str.length());
#else
    std::for_each(str.begin(), str.end(), [](char& ch) {
        CharLowerA(&ch);
    });
#endif
}

void StringToLower(std::wstring& str)
{
    if (str.empty()) {
        return;
    }

#if !defined(_M_X64)
    auto buff = WriteInto(str, str.length() + 1);
    CharLowerBuffW(buff, str.length());
#else
    std::for_each(str.begin(), str.end(), [](wchar_t& ch) {
        CharLowerW(&ch);
    });
#endif
}

void StringToUpper(std::string& str)
{
    if (str.empty()) {
        return;
    }

#if !defined(_M_X64)
    auto buff = WriteInto(str, str.length() + 1);
    CharUpperBuffA(buff, str.length());
#else
    std::for_each(str.begin(), str.end(), [](char& ch) {
        CharUpperA(&ch);
    });
#endif
}

void StringToUpper(std::wstring& str)
{
    if (str.empty()) {
        return;
    }

#if !defined(_M_X64)
    auto buff = WriteInto(str, str.length() + 1);
    CharUpperBuffW(buff, str.length());
#else
    std::for_each(str.begin(), str.end(), [](wchar_t& ch) {
        CharUpperW(&ch);
    });
#endif
}

int StringCompareCaseInsensitive(const std::string& x, const std::string& y)
{
    return _stricmp(x.c_str(), y.c_str());
}

int StringCompareCaseInsensitive(const std::wstring& x, const std::wstring& y)
{
    return _wcsicmp(x.c_str(), y.c_str());
}

int SysStringCompareCaseInsensitive(const std::wstring& x, const std::wstring& y)
{
    int ret = CompareStringOrdinal(x.data(), static_cast<int>(x.length()),
                                   y.data(), static_cast<int>(y.length()), TRUE);
    ENSURE(CHECK, ret != 0)(x)(y).Require();

    return ret - CSTR_EQUAL;
}

bool StartsWith(const std::string& str, StringPiece token, bool case_sensitive)
{
    return StartsWithT(str, token, case_sensitive);
}

bool StartsWith(const std::wstring& str, WStringPiece token, bool case_sensitive)
{
    return StartsWithT(str, token, case_sensitive);
}

bool EndsWith(const std::string& str, StringPiece token, bool case_sensitive)
{
    return EndsWithT(str, token, case_sensitive);
}

bool EndsWith(const std::wstring& str, WStringPiece token, bool case_sensitive)
{
    return EndsWithT(str, token, case_sensitive);
}

size_t SplitString(const std::string& str, StringPiece delimiters,
                   std::vector<std::string>& tokens)
{
    return SplitStringT(str, delimiters, tokens);
}

size_t SplitString(const std::wstring& str, WStringPiece delimiters,
                   std::vector<std::wstring>& tokens)
{
    return SplitStringT(str, delimiters, tokens);
}

std::string JoinString(const std::vector<std::string>& tokens, StringPiece sep)
{
    return JoinStringT(tokens, sep);
}

std::wstring JoinString(const std::vector<std::wstring>& tokens,
                        WStringPiece sep)
{
    return JoinStringT(tokens, sep);
}

bool MatchPattern(const std::string& str, const std::string& pat)
{
    return MatchPatternT(str.c_str(), pat.c_str());
}

bool MatchPattern(const std::wstring& str, const std::wstring& pat)
{
    return MatchPatternT(str.c_str(), pat.c_str());
}

bool IsStringASCII(StringPiece str)
{
    return StringASCIICheck(str);
}

bool IsStringASCII(WStringPiece str)
{
    return StringASCIICheck(str);
}

}   // namespace kbase