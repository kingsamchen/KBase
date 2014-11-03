/*
 @ Kingsley Chen
*/

#include "kbase\strings\string_util.h"

#include <Windows.h>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <functional>

#include "kbase\error_exception_util.h"

namespace {

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

enum TrimPosition {
    TRIM_NONE = 0,
    TRIM_LEADING = 1 << 0,
    TRIM_TAILING = 1 << 1,
    TRIM_ALL = TRIM_LEADING | TRIM_TAILING
};

}   // namespace

namespace kbase {

template<typename strT>
static bool RemoveCharsT(const strT& in,
                         const BasicStringPiece<strT>& remove_chars, strT* out)
{
    strT tmp(in.size(), 0);

    auto char_in = [=](typename strT::value_type ch) -> bool {
        return std::find(remove_chars.cbegin(), remove_chars.cend(), ch) 
            != remove_chars.cend();
    };

    auto new_end = std::remove_copy_if(in.cbegin(), in.cend(), tmp.begin(), char_in);
    tmp.erase(new_end, tmp.end());
    bool did_remove = tmp.size() != in.size();
    using std::swap;
    swap(tmp, *out);

    return did_remove;
}

bool RemoveChars(const std::string& in,
                 const StringPiece& remove_chars,
                 std::string* out)
{
    return RemoveCharsT<std::string>(in, remove_chars, out);
}

bool RemoveChars(const std::wstring& in,
                 const WStringPiece& remove_chars,
                 std::wstring* out)
{
    return RemoveCharsT<std::wstring>(in, remove_chars, out);
}

template<typename strT>
static void ReplaceSubstrHelper(strT* str,
                                const BasicStringPiece<strT>& find_with,
                                const BasicStringPiece<strT>& replace_with,
                                typename strT::size_type pos,
                                bool replace_all)
{
    if (pos == strT::npos || pos + find_with.length() > str->length()) {
        return;
    }

    typename strT::size_type offset = pos;
    while ((offset = str->find(find_with.data(), offset, find_with.length()))
           != strT::npos) {
        str->replace(offset, find_with.length(),
                     replace_with.data(), replace_with.length());
        offset += replace_with.length();

        if (!replace_all) break;
    }
}

void ReplaceSubstring(std::string* str,
                      const StringPiece& find_with,
                      const StringPiece& replace_with,
                      std::string::size_type pos)
{
    ReplaceSubstrHelper(str, find_with, replace_with, pos, true);
}

void ReplaceSubstring(std::wstring* str,
                      const WStringPiece& find_with,
                      const WStringPiece& replace_with,
                      std::wstring::size_type pos)
{
    ReplaceSubstrHelper(str, find_with, replace_with, pos, true);
}

void ReplaceFirstSubstring(std::string* str,
                           const StringPiece& find_with,
                           const StringPiece& replace_with,
                           std::string::size_type pos)
{
    ReplaceSubstrHelper(str, find_with, replace_with, pos, false);
}

void ReplaceFirstSubstring(std::wstring* str,
                           const WStringPiece& find_with,
                           const WStringPiece& replace_with,
                           std::wstring::size_type pos)
{
    ReplaceSubstrHelper(str, find_with, replace_with, pos, false);
}

template<typename strT>
static TrimPosition TrimStringHelper(const strT& in,
                                     const BasicStringPiece<strT> trim_chars,
                                     TrimPosition pos, strT* out)
{
    typename strT::size_type last = in.length() - 1;
    typename strT::size_type not_matched_first = (pos & TrimPosition::TRIM_LEADING) ?
        in.find_first_not_of(trim_chars.data(), 0, trim_chars.length()) : 0;
    typename strT::size_type not_matched_last = (pos & TrimPosition::TRIM_TAILING) ?
        in.find_last_not_of(trim_chars.data(), strT::npos, trim_chars.size()) : last;

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

bool TrimString(const std::string& in,
                const StringPiece& trim_chars,
                std::string* out)
{
    return TrimStringHelper(in, trim_chars, TrimPosition::TRIM_ALL, out) !=
        TrimPosition::TRIM_NONE;
}

bool TrimString(const std::wstring& in,
                const WStringPiece& trim_chars,
                std::wstring* out)
{
    return TrimStringHelper(in, trim_chars, TrimPosition::TRIM_ALL, out) !=
        TrimPosition::TRIM_NONE;
}

bool TrimLeadingStr(const std::string& in,
                    const StringPiece& trim_chars,
                    std::string* out)
{
    return TrimStringHelper(in, trim_chars, TrimPosition::TRIM_LEADING, out) ==
        TrimPosition::TRIM_LEADING;
}

bool TrimLeadingStr(const std::wstring& in,
                    const WStringPiece& trim_chars,
                    std::wstring* out)
{
    return TrimStringHelper(in, trim_chars, TrimPosition::TRIM_LEADING, out) ==
        TrimPosition::TRIM_LEADING;
}

bool TrimTailingStr(const std::string& in,
                    const StringPiece& trim_chars,
                    std::string* out)
{
    return TrimStringHelper(in, trim_chars, TrimPosition::TRIM_TAILING, out) ==
        TrimPosition::TRIM_TAILING;
}

bool TrimTailingStr(const std::wstring& in,
                    const WStringPiece& trim_chars,
                    std::wstring* out)
{
    return TrimStringHelper(in, trim_chars, TrimPosition::TRIM_TAILING, out) ==
        TrimPosition::TRIM_TAILING;
}

template<typename strT>
static bool ContainsOnlyCharsT(const strT& in, const BasicStringPiece<strT>& chars)
{
    return in.find_first_not_of(chars.data(), 0, chars.size()) == strT::npos;
}

bool ContainsOnlyChars(const std::string& in, const StringPiece& chars)
{
    return ContainsOnlyCharsT(in, chars);
}

bool ContainsOnlyChars(const std::wstring& in, const WStringPiece& chars)
{
    return ContainsOnlyCharsT(in, chars);
}

void StringToLower(std::string* str)
{
    if (str->empty()) {
        return;
    }

    auto buff = WriteInto(str, str->length() + 1);
    CharLowerBuffA(buff, str->length()); 
}

std::string StringToLower(const std::string& str)
{
    std::string tmp(str);
    StringToLower(&tmp);

    return tmp;
}

void StringToLower(std::wstring* str)
{
    if (str->empty()) {
        return;
    }

    auto buff = WriteInto(str, str->length() + 1);
    CharLowerBuffW(buff, str->length());
}

std::wstring StringToLower(const std::wstring& str)
{
    std::wstring tmp(str);
    StringToLower(&tmp);

    return tmp;
}

void StringToUpper(std::string* str)
{
    if (str->empty()) {
        return;
    }

    auto buff = WriteInto(str, str->length() + 1);
    CharUpperBuffA(buff, str->length());
}

std::string StringToUpper(const std::string& str)
{
    std::string tmp(str);
    StringToUpper(&tmp);

    return tmp;
}

void StringToUpper(std::wstring* str)
{
    if (str->empty()) {
        return;
    }

    auto buff = WriteInto(str, str->length() + 1);
    CharUpperBuffW(buff, str->length());
}

std::wstring StringToUpper(const std::wstring& str)
{
    std::wstring tmp(str);
    StringToUpper(&tmp);

    return tmp;
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
    int ret = CompareStringOrdinal(x.data(), x.length(), y.data(), y.length(), TRUE);
    ENSURE(ret != 0)(x)(y).raise();

    return ret - CSTR_EQUAL;
}

template<typename strT>
static bool StartsWithT(const strT& str, const strT& token, bool case_sensitive)
{
    if (str.length() < token.length())
        return false;

    if (case_sensitive) {
        return str.compare(0, token.length(), token) == 0;
    } else {
        auto icmp = [](typename strT::value_type lhs, typename strT::value_type rhs) {
            return ToLower(lhs) == ToLower(rhs);
        };

        return std::equal(token.cbegin(), token.cend(), str.cbegin(), icmp); 
    }
}

bool StartsWith(const std::string& str,
                const std::string& token,
                bool case_sensitive)
{
    return StartsWithT(str, token, case_sensitive);
}

bool StartsWith(const std::wstring& str,
                const std::wstring& token,
                bool case_sensitive)
{
    return StartsWithT(str, token, case_sensitive);
}

template<typename strT>
static bool EndsWithT(const strT& str, const strT& token, bool case_sensitive)
{
    if (str.length() < token.length())
        return false;

    typename strT::size_type offset = str.length() - token.length();
    if (case_sensitive) {
        return str.compare(offset, token.length(), token)
            == 0;
    } else {
        auto icmp = [](typename strT::value_type lhs, typename strT::value_type rhs) {
            return ToLower(lhs) == ToLower(rhs);
        };

        return std::equal(token.cbegin(), token.cend(), str.cbegin() + offset, icmp);
    }
}

bool EndsWith(const std::string& str,
              const std::string& token,
              bool case_sensitive)
{
    return EndsWithT(str, token, case_sensitive);
}

bool EndsWith(const std::wstring& str,
              const std::wstring& token,
              bool case_sensitive)
{
    return EndsWithT(str, token, case_sensitive);
}

template<typename strT>
static size_t TokenizeT(const strT& str, const strT& delimiters,
                        std::vector<strT>* tokens)
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

template<typename strT>
static strT JoinStringT(const std::vector<strT>& tokens, const strT& sep)
{
    if (tokens.empty())
        return strT();

    strT str(tokens[0]);
    for (auto it = tokens.cbegin() + 1; it != tokens.cend(); ++it) {
        str.append(sep);
        str.append(*it);
    }

    return str;
}

std::string JoinString(const std::vector<std::string>& tokens, char sep)
{
    return JoinStringT(tokens, std::string(sep, 1));
}

std::wstring JoinString(const std::vector<std::wstring>& tokens, wchar_t sep)
{
    return JoinStringT(tokens, std::wstring(sep, 1));
}

std::string JoinString(const std::vector<std::string>& tokens, const std::string& sep)
{
    return JoinStringT(tokens, sep);
}

std::wstring JoinString(const std::vector<std::wstring>& tokens,
                        const std::wstring& sep)
{
    return JoinStringT(tokens, sep);
}

template<typename charT>
static bool MatchPatternT(const charT* str, const charT* pat)
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

bool MatchPattern(const std::string& str, const std::string& pat)
{
    return MatchPatternT(str.c_str(), pat.c_str());
}

bool MatchPattern(const std::wstring& str, const std::wstring& pat)
{
    return MatchPatternT(str.c_str(), pat.c_str());
}

template<typename strT>
static bool StringASCIICheck(const strT& str)
{
    for (size_t i = 0; i < str.length(); ++i) {
        typename ToUnsigned<typename strT::value_type>::Unsigned c = str[i];
        if (c > 0x7F) {
            return false;
        }
    }

    return true;
}

bool IsStringASCII(const StringPiece& str)
{
    return StringASCIICheck(str);
}

bool IsStringASCII(const WStringPiece& str)
{
    return StringASCIICheck(str);
}

}   // namespace kbase