/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\md5.h"

namespace {

const std::pair<std::string, std::string> hash_pairs[] {
    {"The quick brown fox jumps over the lazy dog", "9e107d9d372bb6826bd81d3542a419d6"}
};

TEST(MD5Test, IncrementHash)
{
    kbase::MD5Digest ret;
    kbase::MD5Context context;
    kbase::MD5Init(&context);
    kbase::MD5Update(&context, hash_pairs[0].first.data(), hash_pairs[0].first.size());
    kbase::MD5Final(&context, &ret);
}

}   // namespace