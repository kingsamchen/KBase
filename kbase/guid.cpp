/*
 @ 0xCCCCCCCC
*/

#include "kbase\guid.h"

#include <objbase.h>

#include <array>
#include <cassert>
#include <xutility>

#include "kbase\string_piece.h"
#include "kbase\sys_string_encoding_conversions.h"

namespace kbase {

std::string GenerateGUID()
{
    GUID guid;
    auto rv = CoCreateGuid(&guid);
    assert(SUCCEEDED(rv));
    if (!SUCCEEDED(rv)) {
        return std::string();
    }

    const int kGUIDStrSize = 40;
    std::array<wchar_t, kGUIDStrSize> guid_str;
    int count_written = StringFromGUID2(guid, guid_str.data(), kGUIDStrSize);

    // Since GUID contains ASCII-only characters, it is safe to do this conversion.
    // Strips off { and }.
    return count_written ?
        WideToASCII(WStringPiece(guid_str.data() + 1, count_written - 3)) :
        std::string();
}

bool IsGUIDValid(const std::string& guid)
{
    // Without { prepended and } appended.
    const size_t kGUIDSize = 36;
    if (guid.length() != kGUIDSize) {
        return false;
    }

    // ord('0') ~ ord('9'), ord('A') ~ ord('F')
    std::pair<int, int> ranges[] {{48, 57}, {65, 70}};
    for (size_t i = 0; i < guid.length(); ++i) {
        if (guid[i] == '-') {
            if (i == 8 || i == 13 || i == 18 || i == 23) {
                continue;
            }

            return false;
        }

        int ascii_code = static_cast<int>(guid[i]);
        if ((ascii_code < ranges[0].first || ascii_code > ranges[0].second) &&
            (ascii_code < ranges[1].first || ascii_code > ranges[1].second)) {
            return false;
        }
    }

    return true;
}

}   // namespace kbase