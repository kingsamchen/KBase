
#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\files\file_util.h"

#include <iostream>
#include <utility>

using namespace kbase;

TEST(FileUtilTest, MakeAbsoluteFilePath)
{
    const FilePath path_tags[] { FilePath(L"."), FilePath(L"..") };
    for (const auto& tag : path_tags) {
        auto&& abs_path = MakeAbsoluteFilePath(tag);
        std::wcout << abs_path.value() << std::endl;
        EXPECT_NE(abs_path, tag);
    }
}

TEST(FileUtilTest, PathExists)
{
    typedef std::pair<FilePath, bool> TestPathExistsPair;
    TestPathExistsPair test_exists[] {
        { FilePath(L"C:\\path_exists_test.fuck"), false },
        { FilePath(L"C:\\Windows"), true }
    };

    for (const auto& p : test_exists) {
        EXPECT_EQ(PathExists(p.first), p.second);
    }
}

