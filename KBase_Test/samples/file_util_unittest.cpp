/*
 @ Kingsley Chen
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

const FilePath dir = PathService::Get(DIR_CURRENT).AppendTo(L"abc_test");
const FilePath file_path = dir.AppendTo(L"abc.txt");

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

TEST(FileUtilTest, DirExists)
{
    typedef std::pair<FilePath, bool> TestPathExistsPair;
    TestPathExistsPair test_exists[] {
            {FilePath(L"C:\\path_exists_test.fuck"), false},
            {FilePath(L"C:\\Windows"), true},
            {FilePath(L"C:\\Windows\\WindowsUpdate.log"), false}
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
    typedef std::pair<FilePath, bool> DirProperty;
    DirProperty dir_pair[] {
        {FilePath(L"C:\\Windows"), false},
        {FilePath(L"C:\\test"), true},
        {FilePath(L"C:\\DirTest"), true}
    };

    for (const auto& p : dir_pair) {
        EXPECT_EQ(IsDirectoryEmpty(p.first), p.second);
    }

    RemoveFile(FilePath(L"C:\\DirTest"), false);
}

TEST(FileUtilTest, DuplicateFile)
{
    CreateDirectoryWithFile();
    ASSERT_TRUE(PathExists(dir));
    ASSERT_TRUE(PathExists(file_path));
    FilePath new_file = dir.BaseName().AppendTo(L"new_file.lala");
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
    FilePath new_dir(L"C:\\moved_dir");
    FilePath new_file = new_dir.AppendTo(file_path.BaseName());
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
    FilePath new_dir(L"C:\\moved_dir");
    FilePath new_file = new_dir.AppendTo(file_path.BaseName());
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
    FilePath file_path(L"file_io_test");
    WriteStringToFile(file_path, original_contents);
    ASSERT_TRUE(PathExists(file_path));
    std::string contents = ReadFileToString(file_path);
    EXPECT_NE(original_contents, contents);
    kbase::RemoveChars(contents, "\r", &contents);
    EXPECT_EQ(original_contents, contents);
    RemoveFile(file_path, false);
}