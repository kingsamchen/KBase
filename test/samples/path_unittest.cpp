/*
 @ 0xCCCCCCCC
*/

#include <algorithm>
#include <functional>

#include "gtest/gtest.h"

#include "kbase/basic_types.h"
#include "kbase/path.h"

using std::placeholders::_1;
using kbase::PathString;
using kbase::Path;

TEST(PathTest, Ctor)
{
    PathString test_path = PATH_LITERAL("C:\\abc\\def");
    Path path(test_path);
    Path another_path(PATH_LITERAL("C:\\test"));

    UNUSED_VAR(path);
    UNUSED_VAR(another_path);
}

TEST(PathTest, PathSeparator)
{
    // IsSeparator()
    {
        EXPECT_TRUE(Path::IsSeparator(PATH_LITERAL('\\')));
        EXPECT_TRUE(Path::IsSeparator(PATH_LITERAL('/')));
        EXPECT_FALSE(Path::IsSeparator(PATH_LITERAL('|')));
    }

    // EndsWithSeperator()
    {
        EXPECT_FALSE(Path().EndsWithSeparator());
        EXPECT_FALSE(Path(PATH_LITERAL("C:\\abc")).EndsWithSeparator());
        EXPECT_FALSE(Path(PATH_LITERAL("/home")).EndsWithSeparator());
        EXPECT_TRUE(Path(PATH_LITERAL("C:\\")).EndsWithSeparator());
        EXPECT_TRUE(Path(PATH_LITERAL("C:\\abc\\")).EndsWithSeparator());
        EXPECT_TRUE(Path(PATH_LITERAL("/usr/")).EndsWithSeparator());
    }

    // StripTrailingSeparators()
    {
        EXPECT_EQ(Path(PATH_LITERAL("C:\\test\\demo")).value(),
                  Path(PATH_LITERAL("C:\\test\\demo\\")).StripTrailingSeparators().value());
        EXPECT_EQ(Path(PATH_LITERAL("C:\\test\\demo")).value(),
                  Path(PATH_LITERAL("C:\\test\\demo")).StripTrailingSeparators().value());

        EXPECT_EQ(Path(PATH_LITERAL("/home/kc")).value(),
                  Path(PATH_LITERAL("/home/kc/")).StripTrailingSeparators().value());
        EXPECT_EQ(Path(PATH_LITERAL("/home/kc")).value(),
                  Path(PATH_LITERAL("/home/kc")).StripTrailingSeparators().value());

        EXPECT_EQ(Path(PATH_LITERAL("C:\\")).value(),
                  Path(PATH_LITERAL("C:\\")).StripTrailingSeparators().value());
        EXPECT_EQ(Path(PATH_LITERAL("C:/")).value(),
                  Path(PATH_LITERAL("C:/")).StripTrailingSeparators().value());
        EXPECT_EQ(Path(PATH_LITERAL("C:")).value(),
                  Path(PATH_LITERAL("C:")).StripTrailingSeparators().value());
        EXPECT_EQ(Path(PATH_LITERAL("C:abc")).value(),
                  Path(PATH_LITERAL("C:abc")).StripTrailingSeparators().value());

        EXPECT_EQ(Path(PATH_LITERAL("/")).value(),
                  Path(PATH_LITERAL("/")).StripTrailingSeparators().value());
    }

    // MakePreferredSeparator() and MakePathSeparatorTo()
    {
#if defined(OS_WIN)
        EXPECT_EQ(Path(PATH_LITERAL("C:\\test\\path")).value(),
                  Path(PATH_LITERAL("C:/test/path")).MakePreferredSeparator().value());
        EXPECT_EQ(Path(PATH_LITERAL("C:\\test\\path")).value(),
                  Path(PATH_LITERAL("C:\\test\\path")).MakePreferredSeparator().value());
#else
        EXPECT_EQ(Path(PATH_LITERAL("C:/test/path")).value(),
                  Path(PATH_LITERAL("C:/test/path")).MakePreferredSeparator().value());
        EXPECT_EQ(Path(PATH_LITERAL("C:/test/path")).value(),
                  Path(PATH_LITERAL("C:\\test\\path")).MakePreferredSeparator().value());
#endif
        EXPECT_EQ(Path(PATH_LITERAL("C:/test/path")).value(),
                  Path(PATH_LITERAL("C:\\test\\path")).MakePathSeparatorTo(PATH_LITERAL('/')).value());
        EXPECT_EQ(Path(PATH_LITERAL("C:test.txt")).value(),
                  Path(PATH_LITERAL("C:test.txt")).MakePreferredSeparator().value());
    }
}

TEST(PathTest, ParentPath)
{
    // Top-pathes
    EXPECT_EQ(Path().value(), Path(PATH_LITERAL("")).parent_path().value());
    EXPECT_EQ(Path().value(), Path(PATH_LITERAL(".")).parent_path().value());
    EXPECT_EQ(Path().value(), Path(PATH_LITERAL("..")).parent_path().value());
    EXPECT_EQ(Path().value(), Path(PATH_LITERAL("abc")).parent_path().value());
    EXPECT_EQ(Path().value(), Path(PATH_LITERAL("C:\\")).parent_path().value());
    EXPECT_EQ(Path().value(), Path(PATH_LITERAL("C:/")).parent_path().value());
    EXPECT_EQ(Path().value(), Path(PATH_LITERAL("C:")).parent_path().value());
    EXPECT_EQ(Path().value(), Path(PATH_LITERAL("/")).parent_path().value());

    EXPECT_EQ(Path(PATH_LITERAL("..")).value(), Path(PATH_LITERAL("../abc")).parent_path().value());
    EXPECT_EQ(Path(PATH_LITERAL(".")).value(), Path(PATH_LITERAL(".\\abc")).parent_path().value());
    EXPECT_EQ(Path(PATH_LITERAL(".")).value(), Path(PATH_LITERAL("./abc")).parent_path().value());

    EXPECT_EQ(Path(PATH_LITERAL("C:\\test\\path")).value(),
              Path(PATH_LITERAL("C:\\test\\path\\data.txt")).parent_path().value());
    EXPECT_EQ(Path(PATH_LITERAL("/")).value(),
              Path(PATH_LITERAL("/home")).parent_path().value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\")).value(),
              Path(PATH_LITERAL("C:\\tmp.txt")).parent_path().value());
    EXPECT_EQ(Path(PATH_LITERAL("C:")).value(),
              Path(PATH_LITERAL("C:tmp.txt")).parent_path().value());
    EXPECT_EQ(Path(PATH_LITERAL("C:/")).value(),
              Path(PATH_LITERAL("C:/tmp.txt")).parent_path().value());
    EXPECT_EQ(Path(PATH_LITERAL("\\\\")).value(),
              Path(PATH_LITERAL("\\\\server\\")).parent_path().value());
}

TEST(PathTest, FileName)
{
    EXPECT_EQ(Path().value(), Path().filename().value());
    EXPECT_EQ(Path(PATH_LITERAL(".")).value(), Path(PATH_LITERAL(".")).filename().value());
    EXPECT_EQ(Path(PATH_LITERAL("..")).value(), Path(PATH_LITERAL("..")).filename().value());
    EXPECT_EQ(Path(PATH_LITERAL("abc")).value(), Path(PATH_LITERAL("abc")).filename().value());
    EXPECT_EQ(Path(PATH_LITERAL("abc")).value(), Path(PATH_LITERAL("./abc")).filename().value());
    EXPECT_EQ(Path(PATH_LITERAL("abc")).value(), Path(PATH_LITERAL("C:\\abc")).filename().value());
    EXPECT_EQ(Path(PATH_LITERAL("foo")).value(), Path(PATH_LITERAL("C:\\foo\\")).filename().value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\")).value(), Path(PATH_LITERAL("C:\\")).filename().value());
    EXPECT_EQ(Path(PATH_LITERAL("C:")).value(), Path(PATH_LITERAL("C:")).filename().value());
    EXPECT_EQ(Path(PATH_LITERAL("tmp.txt")).value(), Path(PATH_LITERAL("C:tmp.txt")).filename().value());
    EXPECT_EQ(Path(PATH_LITERAL("/")).value(), Path(PATH_LITERAL("/")).filename().value());
}

TEST(PathTest, PathComponents)
{
    typedef std::pair<Path, std::vector<Path::string_type>> PathComponentPair;
    PathComponentPair componnet_test[] {
        { Path(PATH_LITERAL("C:tmp.txt")), { PATH_LITERAL("C:"), PATH_LITERAL("tmp.txt") } },
        { Path(PATH_LITERAL("C:\\tmp.txt")), { PATH_LITERAL("C:"), PATH_LITERAL("\\"), PATH_LITERAL("tmp.txt") } },
        { Path(PATH_LITERAL("C:\\foo\\bar")), { PATH_LITERAL("C:"), PATH_LITERAL("\\"), PATH_LITERAL("foo"), PATH_LITERAL("bar") } },
        { Path(PATH_LITERAL("..\\abc")), { PATH_LITERAL(".."), PATH_LITERAL("abc") } },
        { Path(PATH_LITERAL("/home/kc/demo.txt")), { PATH_LITERAL("/"), PATH_LITERAL("home"), PATH_LITERAL("kc"), PATH_LITERAL("demo.txt") } }
    };

    std::vector<Path::string_type> comp;
    for (const auto& p : componnet_test) {
        p.first.GetComponents(comp);
        EXPECT_EQ(p.second, comp);
    }
}

TEST(PathTest, IsAbsolutePath)
{
    EXPECT_EQ(false, Path(PATH_LITERAL(".")).IsAbsolute());
    EXPECT_EQ(false, Path(PATH_LITERAL("abc")).IsAbsolute());
    EXPECT_EQ(false, Path(PATH_LITERAL("./abc")).IsAbsolute());
    EXPECT_EQ(false, Path(PATH_LITERAL("..")).IsAbsolute());
    EXPECT_EQ(false, Path(PATH_LITERAL("../abc")).IsAbsolute());

#if defined(OS_WIN)
    EXPECT_EQ(false, Path(PATH_LITERAL("C:abc")).IsAbsolute());
    EXPECT_EQ(true, Path(PATH_LITERAL("C://abc")).IsAbsolute());
    EXPECT_EQ(true, Path(PATH_LITERAL("\\\\KINGSLEYCHEN-PC\test")).IsAbsolute());
#else
    EXPECT_EQ(true, Path(PATH_LITERAL("/")).IsAbsolute());
    EXPECT_EQ(true, Path(PATH_LITERAL("/home/kingsamchen")).IsAbsolute());
#endif
}

TEST(PathTest, PathAppend)
{
#if defined(OS_WIN)
    EXPECT_EQ(Path(PATH_LITERAL("C:abc")).value(), Path(PATH_LITERAL("C:")).AppendWithASCII("abc").value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc")).value(), Path(PATH_LITERAL("C:\\")).AppendWithASCII("abc").value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def")).value(), Path(PATH_LITERAL("C:\\abc")).AppendWithASCII("def").value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def")).value(), Path(PATH_LITERAL("C:\\abc\\")).AppendWithASCII("def").value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def")).value(), Path(PATH_LITERAL("C:\\")).AppendWithASCII("abc\\def").value());
#else
    EXPECT_EQ(Path(PATH_LITERAL("/home")).value(), Path(PATH_LITERAL("/")).AppendWith("home").value());
    EXPECT_EQ(Path(PATH_LITERAL("/home/kc/projects")).value(), Path(PATH_LITERAL("/home")).AppendWith("kc/projects").value());
    EXPECT_EQ(Path(PATH_LITERAL("/home/kc/projects")).value(), Path(PATH_LITERAL("/home/")).AppendWith("kc/projects").value());
#endif
}

TEST(PathTest, RelativePathAppend)
{
#if defined(OS_WIN)
    Path current(PATH_LITERAL("C:\\user\\kingsley chen"));
    Path child(PATH_LITERAL("C:\\user\\kingsley chen\\app data\\test"));
    Path path(PATH_LITERAL("C:\\user\\kingsley chen\\documents"));
    EXPECT_TRUE(current.AppendRelativePath(child, &path));
    EXPECT_EQ(path.value(), Path(PATH_LITERAL("C:\\user\\kingsley chen\\documents\\app data\\test")).value());
#else
    Path current(PATH_LITERAL("/home/kingsamchen"));
    Path child(PATH_LITERAL("/home/kingsamchen/app-data/test"));
    Path path(PATH_LITERAL("/home/kingsamchen/documents"));
    EXPECT_TRUE(current.AppendRelativePath(child, &path));
    EXPECT_EQ(path.value(), Path(PATH_LITERAL("/home/kingsamchen/documents/app-data/test")).value());
#endif
}

TEST(PathTest, IsParent)
{
    EXPECT_TRUE(Path(PATH_LITERAL("./abc")).IsParent(Path(PATH_LITERAL("./abc/def"))));
    EXPECT_TRUE(Path(PATH_LITERAL("C:")).IsParent(Path(PATH_LITERAL("C:abc.txt"))));
    EXPECT_TRUE(Path(PATH_LITERAL("C:\\")).IsParent(Path(PATH_LITERAL("C:\\abc"))));
    EXPECT_FALSE(Path(PATH_LITERAL("C:\\test\\")).IsParent(Path(PATH_LITERAL("C:\\abc\\"))));

    EXPECT_TRUE(Path(PATH_LITERAL("/")).IsParent(Path(PATH_LITERAL("/home/kc"))));
    EXPECT_TRUE(Path(PATH_LITERAL("/usr/")).IsParent(Path(PATH_LITERAL("/usr/bin"))));
}

TEST(PathTest, ReferenceParent)
{
    EXPECT_FALSE(Path(PATH_LITERAL("./abc")).ReferenceParent());
    EXPECT_FALSE(Path(PATH_LITERAL("./")).ReferenceParent());
    EXPECT_TRUE(Path(PATH_LITERAL("..")).ReferenceParent());
    EXPECT_TRUE(Path(PATH_LITERAL("../abc")).ReferenceParent());
}

TEST(PathTest, PathExtension)
{
    using string_type = Path::string_type;

    EXPECT_EQ(string_type(PATH_LITERAL(".dat")), Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).extension());
    EXPECT_EQ(string_type(PATH_LITERAL(".log")), Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat.log")).extension());
    EXPECT_EQ(string_type(PATH_LITERAL("")), Path(PATH_LITERAL("C:\\abc\\def\\xxx")).extension());

    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).RemoveExtension().value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat.log")).RemoveExtension().value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx")).RemoveExtension().value());

    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx")).AddExtension(PATH_LITERAL("dat")).value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx")).AddExtension(PATH_LITERAL(".dat")).value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat.log")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).AddExtension(PATH_LITERAL(".log")).value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat.log")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx")).AddExtension(PATH_LITERAL(".dat.log")).value());

    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx.log")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).ReplaceExtension(PATH_LITERAL(".log")).value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx.log")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).ReplaceExtension(PATH_LITERAL("log")).value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).ReplaceExtension(PATH_LITERAL(".")).value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).ReplaceExtension(PATH_LITERAL("")).value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx")).ReplaceExtension(PATH_LITERAL(".dat")).value());
    EXPECT_EQ(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).value(),
              Path(PATH_LITERAL("C:\\abc\\def\\xxx")).ReplaceExtension(PATH_LITERAL("dat")).value());
}

TEST(PathTest, Comparison)
{
    EXPECT_TRUE(Path(PATH_LITERAL("C:\\abc\\def\\test.txt")) == Path(PATH_LITERAL("C:\\abc\\def\\test.txt")));
    EXPECT_TRUE(Path(PATH_LITERAL("C:\\abc\\def\\test.txt")) != Path(PATH_LITERAL("C:\\abc\\def\\")));
    EXPECT_TRUE(Path(PATH_LITERAL("/home/kc")) == Path(PATH_LITERAL("/home/kc")));
    EXPECT_TRUE(Path(PATH_LITERAL("/home/kc")) != Path(PATH_LITERAL("/home/test")));
    EXPECT_FALSE(Path(PATH_LITERAL("/home/kc")) < Path(PATH_LITERAL("/home/kc")));
    EXPECT_TRUE(Path(PATH_LITERAL("/home/kc")) < Path(PATH_LITERAL("/home/zen")));

    // case-sensitivity
#if defined(OS_WIN)
    EXPECT_TRUE(Path(PATH_LITERAL("C:\\abc\\def\\test.txt")) == Path(PATH_LITERAL("c:\\ABC\\def\\TEST.TXT")));
#else
    EXPECT_FALSE(Path(PATH_LITERAL("/home/kc")) == Path(PATH_LITERAL("/HOME/kc")));
#endif
}