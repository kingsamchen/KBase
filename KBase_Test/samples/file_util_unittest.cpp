/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\file_util.h"

#include <iostream>
#include <utility>

#include "kbase\path_service.h"
#include "kbase\string_util.h"

using namespace kbase;

namespace {

const Path dir = PathService::Get(DIR_CURRENT).AppendTo(L"abc_test");
const Path file_path = dir.AppendTo(L"abc.txt");

void CreateDirectoryWithFile()
{
    CreateDirectoryW(dir.value().c_str(), nullptr);
    auto handle =
        CreateFileW(file_path.value().c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                    CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
    }
}

}   // namespace

TEST(FileUtilTest, MakeAbsoluteFilePath)
{
    const Path path_tags[] { Path(L"."), Path(L"..") };
    for (const auto& tag : path_tags) {
        auto&& abs_path = MakeAbsoluteFilePath(tag);
        std::wcout << abs_path.value() << std::endl;
        EXPECT_NE(abs_path, tag);
    }
}

TEST(FileUtilTest, PathExists)
{
    typedef std::pair<Path, bool> TestPathExistsPair;
    TestPathExistsPair test_exists[] {
        { Path(L"C:\\path_exists_test.fuck"), false },
        { Path(L"C:\\Windows"), true }
    };

    for (const auto& p : test_exists) {
        EXPECT_EQ(PathExists(p.first), p.second);
    }
}

TEST(FileUtilTest, DirExists)
{
    typedef std::pair<Path, bool> TestPathExistsPair;
    TestPathExistsPair test_exists[] {
            {Path(L"C:\\path_exists_test.fuck"), false},
            {Path(L"C:\\Windows"), true},
            {Path(L"C:\\Windows\\WindowsUpdate.log"), false}
    };

    for (const auto& p : test_exists) {
        EXPECT_EQ(DirectoryExists(p.first), p.second);
    }
}

TEST(FileUtilTest, RemoveFile)
{
    CreateDirectoryWithFile();
    ASSERT_TRUE(PathExists(dir));
    ASSERT_TRUE(PathExists(file_path));
    RemoveFile(file_path, false);
    RemoveFile(dir, false);
    EXPECT_FALSE(PathExists(dir));
    CreateDirectoryWithFile();
    RemoveFile(dir, true);
    EXPECT_FALSE(PathExists(dir));
}

TEST(FileUtilTest, IsDirectoryEmpty)
{
    CreateDirectoryW(L"C:\\DirTest", nullptr);
    typedef std::pair<Path, bool> DirProperty;
    DirProperty dir_pair[] {
        {Path(L"C:\\Windows"), false},
        {Path(L"C:\\test"), true},
        {Path(L"C:\\DirTest"), true}
    };

    for (const auto& p : dir_pair) {
        EXPECT_EQ(IsDirectoryEmpty(p.first), p.second);
    }

    RemoveFile(Path(L"C:\\DirTest"), false);
}

TEST(FileUtilTest, DuplicateFile)
{
    CreateDirectoryWithFile();
    ASSERT_TRUE(PathExists(dir));
    ASSERT_TRUE(PathExists(file_path));
    Path new_file = dir.filename().AppendTo(L"new_file.lala");
    DuplicateFile(file_path, new_file);
    EXPECT_TRUE(PathExists(new_file));
    RemoveFile(new_file, false);
    RemoveFile(dir, true);
}

TEST(FileUtilTest, DuplicateDirectory)
{
    CreateDirectoryWithFile();
    ASSERT_TRUE(PathExists(dir));
    ASSERT_TRUE(PathExists(file_path));
    Path new_dir(L"C:\\moved_dir");
    Path new_file = new_dir.AppendTo(file_path.filename());
    DuplicateDirectory(dir, new_dir, true);
    EXPECT_TRUE(PathExists(new_dir));
    EXPECT_TRUE(PathExists(new_file));
    RemoveFile(new_dir, true);
}

TEST(FileUtilTest, MakeFileMove)
{
    CreateDirectoryWithFile();
    ASSERT_TRUE(PathExists(dir));
    ASSERT_TRUE(PathExists(file_path));
    Path new_dir(L"C:\\moved_dir");
    Path new_file = new_dir.AppendTo(file_path.filename());
    MakeFileMove(dir, new_dir);
    EXPECT_FALSE(PathExists(dir));
    EXPECT_FALSE(PathExists(file_path));
    EXPECT_TRUE(PathExists(new_dir));
    EXPECT_TRUE(PathExists(new_file));
    RemoveFile(new_dir, true);
}

TEST(FileUtilTest, ReadOrWriteFile)
{
    std::string original_contents = "abc\nblabla\nhelloworld";
    Path file_path(L"file_io_test");
    WriteStringToFile(file_path, original_contents);
    ASSERT_TRUE(PathExists(file_path));
    std::string contents = ReadFileToString(file_path);
    EXPECT_NE(original_contents, contents);
    kbase::RemoveChars(contents, "\r", &contents);
    EXPECT_EQ(original_contents, contents);
    RemoveFile(file_path, false);
}