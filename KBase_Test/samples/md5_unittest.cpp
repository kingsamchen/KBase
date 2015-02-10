/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\md5.h"

namespace {

const std::pair<std::string, std::string> hash_pairs[] {
    {"The quick brown fox jumps over the lazy dog", "9e107d9d372bb6826bd81d3542a419d6"},
    {"The quick brown fox jumps over the lazy dog.", "e4d909c290d0fb1ca068ffaddf22cbd0"}
};

}   // namespace

TEST(MD5Test, IncrementHashAndDigestToString)
{
    kbase::MD5Digest ret;
    kbase::MD5Context context;
    kbase::MD5Init(&context);
    kbase::MD5Update(&context, hash_pairs[0].first.data(), hash_pairs[0].first.size());
    kbase::MD5Final(&context, &ret);
    EXPECT_EQ(hash_pairs[0].second, kbase::MD5DigestToString(ret));
}

TEST(MD5Test, MD5Sum)
{
    kbase::MD5Digest digest;
    for (auto& hp : hash_pairs) {
        kbase::MD5Sum(hp.first.data(), hp.first.size(), &digest);
        EXPECT_EQ(hp.second, kbase::MD5DigestToString(digest));
    }
}

TEST(MD5Test, MD5String)
{
    for (auto& hp : hash_pairs) {
        EXPECT_EQ(hp.second, kbase::MD5String(hp.first));
    }
}