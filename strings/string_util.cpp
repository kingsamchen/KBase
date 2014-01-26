
#include "string_util.h"

#include <algorithm>
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

}   // namespace StringUtil

}   // namespace KBase