/*
 @ 0xCCCCCCCC
*/

#include <iostream>
#include <fstream>

#include "catch2/catch.hpp"

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

TEST_CASE("Making absolute path", "[FileUtil]")
{
    // Paths indicated by . and .. always exist.
    const Path path_tags[] {Path(PATH_LITERAL(".")), Path(PATH_LITERAL(".."))};
    for (const auto& tag : path_tags) {
        auto abs_path = MakeAbsolutePath(tag);
        REQUIRE_FALSE(abs_path.empty());
        std::cout << abs_path.AsUTF8() << " -> " << tag.AsUTF8() << std::endl;
    }
}

TEST_CASE("File or directory existence", "[FileUtil]")
{
    SECTION("check if a file exists")
    {
        Path tmp_dir = PathService::Get(DirTemp);
        REQUIRE(!tmp_dir.empty());

        REQUIRE(PathExists(tmp_dir));

        auto tmp_file = tmp_dir.AppendWith(PATH_LITERAL("nonexist_") + ForcePathString(RandomTime()));
        REQUIRE_FALSE(PathExists(tmp_file));
        std::ofstream(tmp_file.value());
        REQUIRE(PathExists(tmp_file));
    }

    SECTION("check if a directory exists")
    {
        Path tmp_dir = PathService::Get(DirTemp);
        REQUIRE(!tmp_dir.empty());
        REQUIRE(DirectoryExists(tmp_dir));

        auto fake_dir = tmp_dir.AppendWith(PATH_LITERAL("fake"));
        REQUIRE_FALSE(DirectoryExists(fake_dir));

        auto file = tmp_dir.AppendWith(PATH_LITERAL("notdir.dat"));
        std::ofstream(file.value());
        REQUIRE(PathExists(file));
        REQUIRE_FALSE(DirectoryExists(file));
    }

    SECTION("create new directories")
    {
        Path tmp_dir = PathService::Get(DirTemp);
        REQUIRE(!tmp_dir.empty());

        auto make_dir_test = tmp_dir.AppendWith(ForcePathString(RandomTime()))
                                    .AppendWith(ForcePathString(RandomTime()));
        REQUIRE_FALSE(DirectoryExists(make_dir_test));
        REQUIRE(MakeDirectory(make_dir_test));
        REQUIRE(DirectoryExists(make_dir_test));

        // Returns true if the directory already exists.
        REQUIRE(MakeDirectory(make_dir_test));
    }

    SECTION("check if a directory is empty")
    {
        Path tmp_dir = PathService::Get(DirTemp);
        REQUIRE(!tmp_dir.empty());

        auto test_dir = tmp_dir.AppendWith(PATH_LITERAL("test_dir_empty"));
        REQUIRE(MakeDirectory(test_dir));
        REQUIRE(IsDirectoryEmpty(test_dir));
        REQUIRE_FALSE(IsDirectoryEmpty(tmp_dir));
    }
}

TEST_CASE("File or directory manipulations", "[FileUtil]")
{
    SECTION("acquire information for a file")
    {
        Path tmp_dir = PathService::Get(DirTemp);
        REQUIRE(!tmp_dir.empty());

        FileInfo info;
        REQUIRE(GetFileInfo(tmp_dir, info));
        REQUIRE(info.is_directory());
        REQUIRE(info.last_modified_time().value().time_since_epoch().count() > 0);

        Path tmp_file = tmp_dir.AppendWith(ForcePathString(RandomTime()));
        std::ofstream out(tmp_file.value());
        out << "abcdefg";
        out.close();
        REQUIRE(PathExists(tmp_file));
        REQUIRE(GetFileInfo(tmp_file, info));
        REQUIRE_FALSE(info.is_directory());
        REQUIRE(info.file_size() > 0);

        REQUIRE_FALSE(GetFileInfo(tmp_dir.AppendWith(PATH_LITERAL("file_info_none")), info));
    }

    SECTION("remove a file or files in a directory")
    {
        Path tmp_dir = PathService::Get(DirTemp);
        REQUIRE(!tmp_dir.empty());

        // Remove an existing file with non-recursive mode.
        auto remove_test_file = tmp_dir.AppendWith(PATH_LITERAL("for_remove"));
        CreateEmptyFile(remove_test_file);
        REQUIRE(PathExists(remove_test_file));
        REQUIRE(RemoveFile(remove_test_file, false));
        REQUIRE_FALSE(PathExists(remove_test_file));

        // Remove an existing file with recursive mode.
        CreateEmptyFile(remove_test_file);
        REQUIRE(PathExists(remove_test_file));
        REQUIRE(RemoveFile(remove_test_file, true));
        REQUIRE_FALSE(PathExists(remove_test_file));

        // Remove an empty directory.
        auto remove_empty_dir = tmp_dir.AppendWith(PATH_LITERAL("for_remove"));
        REQUIRE(MakeDirectory(remove_empty_dir));
        REQUIRE(RemoveFile(remove_empty_dir, false));
        REQUIRE_FALSE(DirectoryExists(remove_empty_dir));

        auto remove_dir = tmp_dir.AppendWith(PATH_LITERAL("for_remove_recursive"));
        REQUIRE(MakeDirectory(remove_dir));
        std::ofstream(remove_dir.AppendWith(PATH_LITERAL("f1")).value());
        std::ofstream(remove_dir.AppendWith(PATH_LITERAL("f2")).value());
        REQUIRE_FALSE(IsDirectoryEmpty(remove_dir));
        // Failed. Because the directory is not empty.
        REQUIRE_FALSE(RemoveFile(remove_dir, false));
        // Success.
        REQUIRE(RemoveFile(remove_dir, true));
        REQUIRE_FALSE(DirectoryExists(remove_dir));
    }

    SECTION("copy a file")
    {
        Path tmp_dir = PathService::Get(DirTemp);
        REQUIRE(!tmp_dir.empty());

        auto original = tmp_dir.AppendWith(PATH_LITERAL("new_file.dada"));
        CreateEmptyFile(original);
        REQUIRE(PathExists(original));

        auto duped = tmp_dir.AppendWith(PATH_LITERAL("new_file.lala"));
        RemoveFile(duped, false);
        REQUIRE_FALSE(PathExists(duped));
        REQUIRE(DuplicateFile(original, duped));
        REQUIRE(PathExists(duped));
        REQUIRE(DuplicateFile(original, duped));
    }

    SECTION("copy a directory")
    {
        Path tmp_dir = PathService::Get(DirTemp);
        REQUIRE(!tmp_dir.empty());

        // Build a complex directory.
        auto inner_dir = tmp_dir.AppendWith(PATH_LITERAL("dup-outer"))
                                .AppendWith(PATH_LITERAL("dup-innter"));
        REQUIRE(MakeDirectory(inner_dir));
        CreateEmptyFile(inner_dir.AppendWith(PATH_LITERAL("inner-f1")));
        CreateEmptyFile(inner_dir.AppendWith(PATH_LITERAL("inner-f2")));
        CreateEmptyFile(inner_dir.parent_path().AppendWith(PATH_LITERAL("outer-f1")));
        CreateEmptyFile(inner_dir.parent_path().AppendWith(PATH_LITERAL("outer-f2")));

        auto src = tmp_dir.AppendWith(PATH_LITERAL("dup-outer"));
        auto dest = tmp_dir.AppendWith(PATH_LITERAL("dup-outer-ex"));
        REQUIRE(DuplicateDirectory(src, dest, true));

        FileIterator end;
        for (FileIterator sit(src, true), dit(dest, true);
             sit != end && dit != end;
             ++sit, ++dit) {
            REQUIRE(sit->file_path().filename() == dit->file_path().filename());
        }
    }

    SECTION("move a file or a directory")
    {
        Path tmp_dir = PathService::Get(DirTemp);
        REQUIRE(!tmp_dir.empty());

        auto mv_file = tmp_dir.AppendWith(PATH_LITERAL("move_from"));
        CreateEmptyFile(mv_file);
        REQUIRE(PathExists(mv_file));
        auto mved = tmp_dir.AppendWith(PATH_LITERAL("move_to"));
        REQUIRE(MakeFileMove(mv_file, mved));
        REQUIRE(PathExists(mved));

        CreateEmptyFile(mv_file);
        REQUIRE(MakeFileMove(mv_file, mved));
    }
}

TEST_CASE("Read or write a file", "[FileUtil]")
{
    Path tmp_dir = PathService::Get(DirTemp);
    REQUIRE(!tmp_dir.empty());

    auto file_path = tmp_dir.AppendWith(PATH_LITERAL("read_write_test.txt"));

    std::string original_contents = "abc\nblabla\nhelloworld";

    WriteStringToFile(file_path, original_contents);
    REQUIRE(PathExists(file_path));

    std::string contents = ReadFileToString(file_path);
    REQUIRE(original_contents == contents);

    RemoveFile(file_path, false);

#if defined(OS_WIN)
    WriteStringToFile(file_path, original_contents, OpenMode::Text);
    REQUIRE(PathExists(file_path));

    ReadFileToString(file_path, contents);
    REQUIRE(original_contents != contents);
    kbase::EraseChars(contents, "\r");
    REQUIRE(original_contents == contents);
#endif
}

}   // namespace kbase
