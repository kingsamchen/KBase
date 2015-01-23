/*
 @ Kingsley Chen
*/

#include "kbase\guid.h"

#include <objbase.h>

#include <array>
#include <cassert>

#include "kbase\strings\string_piece.h"
#include "kbase\strings\sys_string_encoding_conversions.h"

namespace kbase {

std::string GenerateGUID()
{
    GUID guid;
    auto rv = CoCreateGuid(&guid);
    assert(SUCCEEDED(rv));
    if (!SUCCEEDED(rv)) {
        return std::string();
    }

    const int kGUIDSize = 40;
    std::array<wchar_t, kGUIDSize> guid_str;
    int count_written = StringFromGUID2(guid, guid_str.data(), kGUIDSize);

    // Since GUID contains ASCII-only characters, it is safe to do this conversion.
    // Strips off { and }.
    return count_written ?
        WideToASCII(WStringPiece(guid_str.data() + 1, count_written - 3)) :
        std::string();
}

}   // namespace kbase