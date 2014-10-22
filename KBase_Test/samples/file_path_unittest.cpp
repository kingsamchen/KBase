
#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\files\file_path.h"

#include <algorithm>
#include <functional>

using std::placeholders::_1;
using kbase::FilePath;

namespace {

bool ContainsNullterminator(const FilePath::PathString& path_str)
{
    return std::any_of(path_str.begin(), path_str.end(),
                       std::bind(std::equal_to<wchar_t>(), L'\0', _1));
}

}   // namespace

// Null-terminator character indicates the end of a path.
// |FilePath| internally cuts them off.
TEST(FilePathTest, Ctor)
{
    FilePath::PathString test_path = L"C:\\abc\\def";
    FilePath::PathString test_path2 = test_path + FilePath::PathString(2, L'\0');
    ASSERT_FALSE(ContainsNullterminator(test_path));
    ASSERT_TRUE(ContainsNullterminator(test_path2));

    FilePath path(test_path2);
    EXPECT_FALSE(ContainsNullterminator(path.value()));
}

TEST(FilePathTest, Comparison)
{

}