/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\files\file_path.h"

#include <algorithm>
#include <functional>
#include <tuple>
#include <xutility>

#include "kbase\basic_types.h"

using std::placeholders::_1;
using kbase::PathString;
using kbase::FilePath;

namespace {

typedef std::pair<FilePath, FilePath> PathTestPair;

bool ContainsNullterminator(const PathString& path_str)
{
    return std::any_of(path_str.begin(), path_str.end(),
                       std::bind(std::equal_to<wchar_t>(), L'\0', _1));
}

}   // namespace

// Null-terminator character indicates the end of a path.
// |FilePath| internally cuts them off.
TEST(FilePathTest, Ctor)
{
    PathString test_path = L"C:\\abc\\def";
    PathString test_path2 = test_path + PathString(2, L'\0');
    ASSERT_FALSE(ContainsNullterminator(test_path));
    ASSERT_TRUE(ContainsNullterminator(test_path2));

    FilePath path(test_path2);
    EXPECT_FALSE(ContainsNullterminator(path.value()));
}

TEST(FilePathTest, Comparison)
{
    FilePath path1(L"c:\\abc\\def\\ghi.ado"), path2(L"C:\\ABC\\DEF\\GHI.ado");
    FilePath path3(L"C:\\test\\unk");

    EXPECT_EQ(path1, path2);
    EXPECT_NE(path1, path3);
    EXPECT_LT(path1, path3);
}

TEST(FilePathTest, PathSeparator)
{
    // IsSeparator
    EXPECT_TRUE(FilePath::IsSeparator(L'\\'));
    EXPECT_TRUE(FilePath::IsSeparator(L'/'));
    EXPECT_FALSE(FilePath::IsSeparator(L'|'));

    {
        EXPECT_FALSE(FilePath().EndsWithSeparator());
        EXPECT_TRUE(FilePath().AsEndingWithSeparator().empty());
        FilePath path(L"C:\\test\\path");
        EXPECT_FALSE(path.EndsWithSeparator());
        path = path.AsEndingWithSeparator();
        EXPECT_TRUE(path.EndsWithSeparator());
        EXPECT_FALSE(path.StripTrailingSeparators().EndsWithSeparator());
        FilePath p(L"C:\\test\\path");
        EXPECT_EQ(p, p.StripTrailingSeparators());
    }
    
    {
        FilePath path(L"C:\\test\\path");
        FilePath crit(L"C:/test/path");
        EXPECT_EQ(crit, path.NormalizePathSeparatorTo(L'/'));
        EXPECT_EQ(crit.NormalizePathSeparator(), path);
    }
}

TEST(FilePathTest, PathComponents)
{
    const FilePath kCurrentPath(L".");
    const FilePath kRootDir(L"..");

    FilePath cur_paths[] { FilePath(L""), FilePath(L"."), FilePath(L"abc"), FilePath(L"./abc") };
    for (const auto& path : cur_paths) {
        EXPECT_EQ(kCurrentPath, path.DirName());
    }

    FilePath root_paths[] { FilePath(L"../abc") };
    for (const auto& path : root_paths) {
        EXPECT_EQ(kRootDir, path.DirName());
    }

    FilePath path(L"C:\\test\\path\\data.txt");
    EXPECT_EQ(path.DirName(), FilePath(L"C:\\test\\path"));
    EXPECT_EQ(FilePath(L"C:\\"), FilePath(L"C:\\").DirName());

    PathTestPair base_dir_test[] {
        { FilePath(), FilePath() },
        { FilePath(L"."), FilePath(L".") },
        { FilePath(L"abc"), FilePath(L"abc") },
        { FilePath(L"abc"), FilePath(L"./abc") },
        { FilePath(L"abc"), FilePath(L"C:\\abc") },
        { FilePath(L"\\"), FilePath(L"C:\\") }
    };

    for (const auto& p : base_dir_test) {
        EXPECT_EQ(p.first, p.second.BaseName());
    }

    typedef std::pair<FilePath, std::vector<std::wstring>> PathComponentPair;
    PathComponentPair componnet_test[] {
        { FilePath(L"C:"), { L"C:" } },
        { FilePath(L"C:\\"), { L"C:", L"\\" } },
        { FilePath(L"C:\\foo\\bar"), { L"C:", L"\\", L"foo", L"bar"} }
    };

    std::vector<std::wstring> comp;
    for (const auto& p : componnet_test) {
        p.first.GetComponents(&comp);
        EXPECT_EQ(comp, p.second);
    }
}

TEST(FilePathTest, PathProperty)
{
    typedef std::pair<FilePath, bool> PathPropertyPair;
    
    // IsAbsolte
    PathPropertyPair abs_path[] {
        { FilePath(L"."), false },
        { FilePath(L"abc"), false },
        { FilePath(L"./abc"), false },
        { FilePath(L".."), false },
        { FilePath(L"../abc"), false },
        { FilePath(L"C:abc"), false },
        { FilePath(L"C://abc"), true }
    };

    for (const auto& p : abs_path) {
        EXPECT_EQ(p.first.IsAbsolute(), p.second);
    }

    // ReferenceParent
    PathPropertyPair refer_parent_path[] {
        { FilePath(L"./abc"), false },
        { FilePath(L".."), true },
        { FilePath(L"../abc"), true }
    };

    for (const auto& p : refer_parent_path) {
      EXPECT_EQ(p.first.ReferenceParent(), p.second);
    }

    typedef std::tuple<FilePath, FilePath, bool> PathPropertyTuple;
    PathPropertyTuple paths[] {
        std::make_tuple(FilePath(L"./abc"), FilePath(L"./abc/def"), true),
        std::make_tuple(FilePath(L"C:\\"), FilePath(L"C:\\abc"), true),
        std::make_tuple(FilePath(L"C:\\test\\"), FilePath(L"C:\\abc\\"), false)
    };

    for (const auto& t : paths) {
        EXPECT_EQ(std::get<0>(t).IsParent(std::get<1>(t)), std::get<2>(t));
    }
}

TEST(FilePathTest, PathAppend)
{
    typedef std::tuple<FilePath, FilePath, std::wstring> PathAppendTuple;
    PathAppendTuple paths[] {
        std::make_tuple(FilePath(L"C:abc"), FilePath(L"C:"), L"abc"),
        std::make_tuple(FilePath(L"C:\\abc"), FilePath(L"C:\\"), L"abc"),
        std::make_tuple(FilePath(L"C:\\abc\\def"), FilePath(L"C:\\abc"), L"def")
    };

    for (const auto& t : paths) {
        EXPECT_EQ(std::get<0>(t), std::get<1>(t).AppendTo(std::get<2>(t)));
    }

    FilePath current(LR"(C:\user\kingsley chen\)");
    FilePath child(LR"(C:\user\kingsley chen\app data\test)");
    FilePath path(LR"(C:\user\kingsley chen\documents\)");
    EXPECT_TRUE(current.AppendRelativePath(child, &path));
    EXPECT_EQ(path, FilePath(L"C:\\user\\kingsley chen\\documents\\app data\\test"));
}

TEST(FilePathTest, PathExtension)
{
    FilePath path(L"C:\\abc\\def\\xxx.dat");
    EXPECT_EQ(path.Extension(), std::wstring(L".dat"));
    path.RemoveExtension();
    EXPECT_EQ(path.Extension(), std::wstring());
    EXPECT_EQ(path, FilePath(L"C:\\abc\\def\\xxx"));
    path = path.AddExtension(L".txt");
    EXPECT_EQ(path.Extension(), std::wstring(L".txt"));
    path = path.ReplaceExtension(L".avi");
    EXPECT_EQ(path.Extension(), std::wstring(L".avi"));
    EXPECT_TRUE(path.MatchExtension(L".avi"));
}