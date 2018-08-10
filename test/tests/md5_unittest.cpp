/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include "kbase/md5.h"

namespace kbase {

TEST_CASE("Computing md5 hash value for a chunk data", "[MD5]")
{
    const std::pair<std::string, std::string> hash_pairs[] {
        {"The quick brown fox jumps over the lazy dog", "9e107d9d372bb6826bd81d3542a419d6"},
        {"The quick brown fox jumps over the lazy dog.", "e4d909c290d0fb1ca068ffaddf22cbd0"}
    };

    SECTION("we can calculcate md5 value incrementally")
    {
        kbase::MD5Digest ret;
        kbase::MD5Context context;
        kbase::MD5Init(context);
        kbase::MD5Update(context, hash_pairs[0].first.data(), hash_pairs[0].first.size());
        kbase::MD5Final(context, ret);
        REQUIRE(hash_pairs[0].second == kbase::MD5DigestToString(ret));
    }

    SECTION("we can also get md5 value in one step")
    {
        kbase::MD5Digest digest;
        for (auto& hp : hash_pairs) {
            kbase::MD5Sum(hp.first.data(), hp.first.size(), digest);
            REQUIRE(hp.second == kbase::MD5DigestToString(digest));
        }
    }

    SECTION("get md5 string")
    {
        for (auto& hp : hash_pairs) {
            REQUIRE(hp.second == kbase::MD5String(hp.first));
        }
    }
}

}   // namespace kbase
