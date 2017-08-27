/*
 @ 0xCCCCCCCC
*/

#include <iostream>
#include <fstream>

#include "gtest/gtest.h"

#include "kbase/base_path_provider.h"
#include "kbase/file_iterator.h"
#include "kbase/file_util.h"
#include "kbase/path.h"
#include "kbase/path_service.h"
#include "kbase/string_util.h"

namespace {

using kbase::Path;
using kbase::PathString;

void CreateEmptyFile(const Path& filepath)
{
    std::ofstream(filepath.value());
}

PathString ForcePathString(const std::string& str)
{
#if defined(OS_WIN)
    return kbase::UTF8ToWide(str);
#else
    return str;
#endif
}

std::string RandomTime()
{
    return std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
}

}   // namespace

namespace kbase {

TEST(FileUtilTest, MakeAbsolutePath)
{
    // Paths indicated by . and .. always exist.
    const Path path_tags[] {Path(PATH_LITERAL(".")), Path(PATH_LITERAL(".."))};
    for (const auto& tag : path_tags) {
        auto abs_path = MakeAbsolutePath(tag);
        EXPECT_FALSE(abs_path.empty());
        std::cout << abs_path.AsUTF8() << " -> " << tag.AsUTF8() << std::endl;
    }
}

TEST(FileUtilTest, PathExists)
{
    Path tmp_dir = PathService::Get(DirTemp);
    ASSERT_TRUE(!tmp_dir.empty());

    EXPECT_TRUE(PathExists(tmp_dir));

    auto tmp_file = tmp_dir.AppendWith(PATH_LITERAL("nonexist_") + ForcePathString(RandomTime()));
    EXPECT_FALSE(PathExists(tmp_file));
    std::ofstream(tmp_file.value());
    EXPECT_TRUE(PathExists(tmp_file));
}

TEST(FileUtilTest, DirExists)
{
    Path tmp_dir = PathService::Get(DirTemp);
    ASSERT_TRUE(!tmp_dir.empty());
    EXPECT_TRUE(DirectoryExists(tmp_dir));

    auto fake_dir = tmp_dir.AppendWith(PATH_LITERAL("fake"));
    EXPECT_FALSE(DirectoryExists(fake_dir));

    auto file = tmp_dir.AppendWith(PATH_LITERAL("notdir.dat"));
    std::ofstream(file.value());
    ASSERT_TRUE(PathExists(file));
    EXPECT_FALSE(DirectoryExists(file));
}

TEST(FileUtilTest, MakeDirectory)
{
    Path tmp_dir = PathService::Get(DirTemp);
    ASSERT_TRUE(!tmp_dir.empty());

    auto make_dir_test = tmp_dir.AppendWith(ForcePathString(RandomTime()))
                                .AppendWith(ForcePathString(RandomTime()));
    ASSERT_FALSE(DirectoryExists(make_dir_test));
    EXPECT_TRUE(MakeDirectory(make_dir_test));
    EXPECT_TRUE(DirectoryExists(make_dir_test));

    // Returns true if the directory already exists.
    EXPECT_TRUE(MakeDirectory(make_dir_test));
}

TEST(FileUtilTest, IsDirectoryEmpty)
{
    Path tmp_dir = PathService::Get(DirTemp);
    ASSERT_TRUE(!tmp_dir.empty());

    auto test_dir = tmp_dir.AppendWith(PATH_LITERAL("test_dir_empty"));
    ASSERT_TRUE(MakeDirectory(test_dir));
    EXPECT_TRUE(IsDirectoryEmpty(test_dir));
    EXPECT_FALSE(IsDirectoryEmpty(tmp_dir));
}

TEST(FileUtilTest, GetFileInfo)
{
    Path tmp_dir = PathService::Get(DirTemp);
    ASSERT_TRUE(!tmp_dir.empty());

    FileInfo info;
    ASSERT_TRUE(GetFileInfo(tmp_dir, info));
    EXPECT_TRUE(info.is_directory());
    EXPECT_TRUE(info.last_modified_time().value().time_since_epoch().count() > 0);

    Path tmp_file = tmp_dir.AppendWith(ForcePathString(RandomTime()));
    std::ofstream out(tmp_file.value());
    out << "abcdefg";
    out.close();
    ASSERT_TRUE(PathExists(tmp_file));
    EXPECT_TRUE(GetFileInfo(tmp_file, info));
    EXPECT_FALSE(info.is_directory());
    EXPECT_TRUE(info.file_size() > 0);

    EXPECT_FALSE(GetFileInfo(tmp_dir.AppendWith(PATH_LITERAL("file_info_none")), info));
}

TEST(FileUtilTest, RemoveFile)
{
    Path tmp_dir = PathService::Get(DirTemp);
    ASSERT_TRUE(!tmp_dir.empty());

    // Remove an existing file with non-recursive mode.
    auto remove_test_file = tmp_dir.AppendWith(PATH_LITERAL("for_remove"));
    CreateEmptyFile(remove_test_file);
    ASSERT_TRUE(PathExists(remove_test_file));
    EXPECT_TRUE(RemoveFile(remove_test_file, false));
    EXPECT_FALSE(PathExists(remove_test_file));

    // Remove an existing file with recursive mode.
    CreateEmptyFile(remove_test_file);
    ASSERT_TRUE(PathExists(remove_test_file));
    EXPECT_TRUE(RemoveFile(remove_test_file, true));
    EXPECT_FALSE(PathExists(remove_test_file));

    // Remove an empty directory.
    auto remove_empty_dir = tmp_dir.AppendWith(PATH_LITERAL("for_remove"));
    ASSERT_TRUE(MakeDirectory(remove_empty_dir));
    EXPECT_TRUE(RemoveFile(remove_empty_dir, false));
    EXPECT_FALSE(DirectoryExists(remove_empty_dir));

    auto remove_dir = tmp_dir.AppendWith(PATH_LITERAL("for_remove_recursive"));
    ASSERT_TRUE(MakeDirectory(remove_dir));
    std::ofstream(remove_dir.AppendWith(PATH_LITERAL("f1")).value());
    std::ofstream(remove_dir.AppendWith(PATH_LITERAL("f2")).value());
    ASSERT_FALSE(IsDirectoryEmpty(remove_dir));
    // Failed. Because the directory is not empty.
    EXPECT_FALSE(RemoveFile(remove_dir, false));
    // Success.
    EXPECT_TRUE(RemoveFile(remove_dir, true));
    EXPECT_FALSE(DirectoryExists(remove_dir));
}

TEST(FileUtilTest, DuplicateFile)
{
    Path tmp_dir = PathService::Get(DirTemp);
    ASSERT_TRUE(!tmp_dir.empty());

    auto original = tmp_dir.AppendWith(PATH_LITERAL("new_file.dada"));
    CreateEmptyFile(original);
    ASSERT_TRUE(PathExists(original));

    auto duped = tmp_dir.AppendWith(PATH_LITERAL("new_file.lala"));
    RemoveFile(duped, false);
    ASSERT_FALSE(PathExists(duped));
    EXPECT_TRUE(DuplicateFile(original, duped));
    EXPECT_TRUE(PathExists(duped));
    EXPECT_TRUE(DuplicateFile(original, duped));
}

TEST(FileUtilTest, DuplicateDirectory)
{
    Path tmp_dir = PathService::Get(DirTemp);
    ASSERT_TRUE(!tmp_dir.empty());

    // Build a complex directory.
    auto inner_dir = tmp_dir.AppendWith(PATH_LITERAL("dup-outer"))
                            .AppendWith(PATH_LITERAL("dup-innter"));
    ASSERT_TRUE(MakeDirectory(inner_dir));
    CreateEmptyFile(inner_dir.AppendWith(PATH_LITERAL("inner-f1")));
    CreateEmptyFile(inner_dir.AppendWith(PATH_LITERAL("inner-f2")));
    CreateEmptyFile(inner_dir.parent_path().AppendWith(PATH_LITERAL("outer-f1")));
    CreateEmptyFile(inner_dir.parent_path().AppendWith(PATH_LITERAL("outer-f2")));

    auto src = tmp_dir.AppendWith(PATH_LITERAL("dup-outer"));
    auto dest = tmp_dir.AppendWith(PATH_LITERAL("dup-outer-ex"));
    EXPECT_TRUE(DuplicateDirectory(src, dest, true));

    FileIterator end;
    for (FileIterator sit(src, true), dit(dest, true); sit != end && dit != end; ++sit, ++dit) {
        EXPECT_TRUE(sit->file_path().filename() == dit->file_path().filename());
    }
}

TEST(FileUtilTest, MakeFileMove)
{
    Path tmp_dir = PathService::Get(DirTemp);
    ASSERT_TRUE(!tmp_dir.empty());

    auto mv_file = tmp_dir.AppendWith(PATH_LITERAL("move_from"));
    CreateEmptyFile(mv_file);
    ASSERT_TRUE(PathExists(mv_file));
    auto mved = tmp_dir.AppendWith(PATH_LITERAL("move_to"));
    EXPECT_TRUE(MakeFileMove(mv_file, mved));
    EXPECT_TRUE(PathExists(mved));

    CreateEmptyFile(mv_file);
    EXPECT_TRUE(MakeFileMove(mv_file, mved));
}

TEST(FileUtilTest, ReadOrWriteFile)
{
    Path tmp_dir = PathService::Get(DirTemp);
    ASSERT_TRUE(!tmp_dir.empty());

    auto file_path = tmp_dir.AppendWith(PATH_LITERAL("read_write_test.txt"));

    std::string original_contents = "abc\nblabla\nhelloworld";
    WriteStringToFile(file_path, original_contents);
    EXPECT_TRUE(PathExists(file_path));

    std::string contents = ReadFileToString(file_path);
#if defined(OS_WIN)
    EXPECT_NE(original_contents, contents);
    kbase::EraseChars(contents, "\r");
    EXPECT_EQ(original_contents, contents);
#else
    EXPECT_EQ(original_contents, contents);
#endif

    RemoveFile(file_path, false);
}

}   // namespace kbase
