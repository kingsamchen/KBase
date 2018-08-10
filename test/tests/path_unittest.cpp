/*
 @ 0xCCCCCCCC
*/

#include <algorithm>
#include <functional>

#include "catch2/catch.hpp"

#include "kbase/basic_types.h"
#include "kbase/path.h"

using std::placeholders::_1;

namespace kbase {

TEST_CASE("Constructions", "[Path]")
{
    PathString test_path = PATH_LITERAL("C:\\abc\\def");
    Path path(test_path);
    Path another_path(PATH_LITERAL("C:\\test"));

    UNUSED_VAR(path);
    UNUSED_VAR(another_path);
}

TEST_CASE("Multiple path-separator functionalities are supported", "[Path]")
{
    SECTION("check if a character is a path separator")
    {
        REQUIRE(Path::IsSeparator(PATH_LITERAL('\\')));
        REQUIRE(Path::IsSeparator(PATH_LITERAL('/')));
        REQUIRE_FALSE(Path::IsSeparator(PATH_LITERAL('|')));
    }

    SECTION("check if a path ends with path separator")
    {
        REQUIRE_FALSE(Path().EndsWithSeparator());
        REQUIRE_FALSE(Path(PATH_LITERAL("C:\\abc")).EndsWithSeparator());
        REQUIRE_FALSE(Path(PATH_LITERAL("/home")).EndsWithSeparator());
        REQUIRE(Path(PATH_LITERAL("C:\\")).EndsWithSeparator());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\")).EndsWithSeparator());
        REQUIRE(Path(PATH_LITERAL("/usr/")).EndsWithSeparator());
    }

    SECTION("we can strip trailing separators if we want")
    {
        REQUIRE(Path(PATH_LITERAL("C:\\test\\demo")).value() ==
                Path(PATH_LITERAL("C:\\test\\demo\\")).StripTrailingSeparators().value());
        REQUIRE(Path(PATH_LITERAL("C:\\test\\demo")).value() ==
                Path(PATH_LITERAL("C:\\test\\demo")).StripTrailingSeparators().value());

        REQUIRE(Path(PATH_LITERAL("/home/kc")).value() ==
                Path(PATH_LITERAL("/home/kc/")).StripTrailingSeparators().value());
        REQUIRE(Path(PATH_LITERAL("/home/kc")).value() ==
                Path(PATH_LITERAL("/home/kc")).StripTrailingSeparators().value());

        REQUIRE(Path(PATH_LITERAL("C:\\")).value() ==
                Path(PATH_LITERAL("C:\\")).StripTrailingSeparators().value());
        REQUIRE(Path(PATH_LITERAL("C:/")).value() ==
                Path(PATH_LITERAL("C:/")).StripTrailingSeparators().value());
        REQUIRE(Path(PATH_LITERAL("C:")).value() ==
                Path(PATH_LITERAL("C:")).StripTrailingSeparators().value());
        REQUIRE(Path(PATH_LITERAL("C:abc")).value() ==
                Path(PATH_LITERAL("C:abc")).StripTrailingSeparators().value());

        REQUIRE(Path(PATH_LITERAL("/")).value() ==
                Path(PATH_LITERAL("/")).StripTrailingSeparators().value());
    }

    SECTION("we can normalize path separators to platform preferred one")
    {
#if defined(OS_WIN)
        REQUIRE(Path(PATH_LITERAL("C:\\test\\path")).value() ==
                Path(PATH_LITERAL("C:/test/path")).MakePreferredSeparator().value());
        REQUIRE(Path(PATH_LITERAL("C:\\test\\path")).value() ==
                Path(PATH_LITERAL("C:\\test\\path")).MakePreferredSeparator().value());
#else
        REQUIRE(Path(PATH_LITERAL("C:/test/path")).value() ==
                Path(PATH_LITERAL("C:/test/path")).MakePreferredSeparator().value());
        REQUIRE(Path(PATH_LITERAL("C:/test/path")).value() ==
                Path(PATH_LITERAL("C:\\test\\path")).MakePreferredSeparator().value());
#endif
        REQUIRE(Path(PATH_LITERAL("C:/test/path")).value() ==
                Path(PATH_LITERAL("C:\\test\\path")).MakePathSeparatorTo(PATH_LITERAL('/')).value());
        REQUIRE(Path(PATH_LITERAL("C:test.txt")).value() ==
                Path(PATH_LITERAL("C:test.txt")).MakePreferredSeparator().value());
    }
}

TEST_CASE("Parent path functions", "[Path]")
{
    SECTION("query parent path")
    {
        // Top-pathes
        REQUIRE(Path().value() == Path(PATH_LITERAL("")).parent_path().value());
        REQUIRE(Path().value() == Path(PATH_LITERAL(".")).parent_path().value());
        REQUIRE(Path().value() == Path(PATH_LITERAL("..")).parent_path().value());
        REQUIRE(Path().value() == Path(PATH_LITERAL("abc")).parent_path().value());
        REQUIRE(Path().value() == Path(PATH_LITERAL("C:\\")).parent_path().value());
        REQUIRE(Path().value() == Path(PATH_LITERAL("C:/")).parent_path().value());
        REQUIRE(Path().value() == Path(PATH_LITERAL("C:")).parent_path().value());
        REQUIRE(Path().value() == Path(PATH_LITERAL("/")).parent_path().value());

        REQUIRE(Path(PATH_LITERAL("..")).value() == Path(PATH_LITERAL("../abc")).parent_path().value());
        REQUIRE(Path(PATH_LITERAL(".")).value() == Path(PATH_LITERAL(".\\abc")).parent_path().value());
        REQUIRE(Path(PATH_LITERAL(".")).value() == Path(PATH_LITERAL("./abc")).parent_path().value());

        REQUIRE(Path(PATH_LITERAL("C:\\test\\path")).value() ==
                Path(PATH_LITERAL("C:\\test\\path\\data.txt")).parent_path().value());
        REQUIRE(Path(PATH_LITERAL("/")).value() ==
                Path(PATH_LITERAL("/home")).parent_path().value());
        REQUIRE(Path(PATH_LITERAL("C:\\")).value() ==
                Path(PATH_LITERAL("C:\\tmp.txt")).parent_path().value());
        REQUIRE(Path(PATH_LITERAL("C:")).value() ==
                Path(PATH_LITERAL("C:tmp.txt")).parent_path().value());
        REQUIRE(Path(PATH_LITERAL("C:/")).value() ==
                Path(PATH_LITERAL("C:/tmp.txt")).parent_path().value());
        REQUIRE(Path(PATH_LITERAL("\\\\")).value() ==
                Path(PATH_LITERAL("\\\\server\\")).parent_path().value());
    }

    SECTION("check if a path is parent of another one")
    {
        REQUIRE(Path(PATH_LITERAL("./abc")).IsParent(Path(PATH_LITERAL("./abc/def"))));
        REQUIRE(Path(PATH_LITERAL("C:")).IsParent(Path(PATH_LITERAL("C:abc.txt"))));
        REQUIRE(Path(PATH_LITERAL("C:\\")).IsParent(Path(PATH_LITERAL("C:\\abc"))));
        REQUIRE_FALSE(Path(PATH_LITERAL("C:\\test\\")).IsParent(Path(PATH_LITERAL("C:\\abc\\"))));

        REQUIRE(Path(PATH_LITERAL("/")).IsParent(Path(PATH_LITERAL("/home/kc"))));
        REQUIRE(Path(PATH_LITERAL("/usr/")).IsParent(Path(PATH_LITERAL("/usr/bin"))));
    }

    SECTION("check if reference a parent path")
    {
        REQUIRE_FALSE(Path(PATH_LITERAL("./abc")).ReferenceParent());
        REQUIRE_FALSE(Path(PATH_LITERAL("./")).ReferenceParent());
        REQUIRE(Path(PATH_LITERAL("..")).ReferenceParent());
        REQUIRE(Path(PATH_LITERAL("../abc")).ReferenceParent());
    }
}

TEST_CASE("Query file-name part of a path", "[Path]")
{
    REQUIRE(Path().value() == Path().filename().value());
    REQUIRE(Path(PATH_LITERAL(".")).value() == Path(PATH_LITERAL(".")).filename().value());
    REQUIRE(Path(PATH_LITERAL("..")).value() == Path(PATH_LITERAL("..")).filename().value());
    REQUIRE(Path(PATH_LITERAL("abc")).value() == Path(PATH_LITERAL("abc")).filename().value());
    REQUIRE(Path(PATH_LITERAL("abc")).value() == Path(PATH_LITERAL("./abc")).filename().value());
    REQUIRE(Path(PATH_LITERAL("abc")).value() == Path(PATH_LITERAL("C:\\abc")).filename().value());
    REQUIRE(Path(PATH_LITERAL("foo")).value() == Path(PATH_LITERAL("C:\\foo\\")).filename().value());
    REQUIRE(Path(PATH_LITERAL("C:\\")).value() == Path(PATH_LITERAL("C:\\")).filename().value());
    REQUIRE(Path(PATH_LITERAL("C:")).value() == Path(PATH_LITERAL("C:")).filename().value());
    REQUIRE(Path(PATH_LITERAL("tmp.txt")).value() == Path(PATH_LITERAL("C:tmp.txt")).filename().value());
    REQUIRE(Path(PATH_LITERAL("/")).value() == Path(PATH_LITERAL("/")).filename().value());
}

TEST_CASE("We can get all components that consist of a path", "[Path]")
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
        REQUIRE(p.second == comp);
    }
}

TEST_CASE("Check if a path is absolute path", "[Path]")
{
    REQUIRE(false == Path(PATH_LITERAL(".")).IsAbsolute());
    REQUIRE(false == Path(PATH_LITERAL("abc")).IsAbsolute());
    REQUIRE(false == Path(PATH_LITERAL("./abc")).IsAbsolute());
    REQUIRE(false == Path(PATH_LITERAL("..")).IsAbsolute());
    REQUIRE(false == Path(PATH_LITERAL("../abc")).IsAbsolute());

#if defined(OS_WIN)
    REQUIRE(false == Path(PATH_LITERAL("C:abc")).IsAbsolute());
    REQUIRE(true == Path(PATH_LITERAL("C://abc")).IsAbsolute());
    REQUIRE(true == Path(PATH_LITERAL("\\\\KINGSLEYCHEN-PC\test")).IsAbsolute());
#else
    REQUIRE(true == Path(PATH_LITERAL("/")).IsAbsolute());
    REQUIRE(true == Path(PATH_LITERAL("/home/kingsamchen")).IsAbsolute());
#endif
}

TEST_CASE("Append to a path", "[Path]")
{
    SECTION("normal appending")
    {
#if defined(OS_WIN)
        REQUIRE(Path(PATH_LITERAL("C:abc")).value() == Path(PATH_LITERAL("C:")).AppendWithASCII("abc").value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc")).value() == Path(PATH_LITERAL("C:\\")).AppendWithASCII("abc").value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def")).value() == Path(PATH_LITERAL("C:\\abc")).AppendWithASCII("def").value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def")).value() == Path(PATH_LITERAL("C:\\abc\\")).AppendWithASCII("def").value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def")).value() == Path(PATH_LITERAL("C:\\")).AppendWithASCII("abc\\def").value());
#else
        REQUIRE(Path(PATH_LITERAL("/home")).value() == Path(PATH_LITERAL("/")).AppendWith("home").value());
        REQUIRE(Path(PATH_LITERAL("/home/kc/projects")).value() == Path(PATH_LITERAL("/home")).AppendWith("kc/projects").value());
        REQUIRE(Path(PATH_LITERAL("/home/kc/projects")).value() == Path(PATH_LITERAL("/home/")).AppendWith("kc/projects").value());
#endif
    }

    SECTION("appending as relative path")
    {
#if defined(OS_WIN)
        Path current(PATH_LITERAL("C:\\user\\kingsley chen"));
        Path child(PATH_LITERAL("C:\\user\\kingsley chen\\app data\\test"));
        Path path(PATH_LITERAL("C:\\user\\kingsley chen\\documents"));
        REQUIRE(current.AppendRelativePath(child, &path));
        REQUIRE(path.value() == Path(PATH_LITERAL("C:\\user\\kingsley chen\\documents\\app data\\test")).value());
#else
        Path current(PATH_LITERAL("/home/kingsamchen"));
        Path child(PATH_LITERAL("/home/kingsamchen/app-data/test"));
        Path path(PATH_LITERAL("/home/kingsamchen/documents"));
        REQUIRE(current.AppendRelativePath(child, &path));
        REQUIRE(path.value() == Path(PATH_LITERAL("/home/kingsamchen/documents/app-data/test")).value());
#endif
    }
}

TEST_CASE("Handle extensions", "[Path]")
{
    using string_type = Path::string_type;

    SECTION("obtain extension")
    {
        REQUIRE(string_type(PATH_LITERAL(".dat")) == Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).extension());
        REQUIRE(string_type(PATH_LITERAL(".log")) == Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat.log")).extension());
        REQUIRE(string_type(PATH_LITERAL("")) == Path(PATH_LITERAL("C:\\abc\\def\\xxx")).extension());
    }

    SECTION("remove extension")
    {
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).RemoveExtension().value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat.log")).RemoveExtension().value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx")).RemoveExtension().value());
    }

    SECTION("add extension")
    {
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx")).AddExtension(PATH_LITERAL("dat")).value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx")).AddExtension(PATH_LITERAL(".dat")).value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat.log")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).AddExtension(PATH_LITERAL(".log")).value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat.log")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx")).AddExtension(PATH_LITERAL(".dat.log")).value());
    }

    SECTION("replace extension")
    {
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx.log")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).ReplaceExtension(PATH_LITERAL(".log")).value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx.log")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).ReplaceExtension(PATH_LITERAL("log")).value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).ReplaceExtension(PATH_LITERAL(".")).value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).ReplaceExtension(PATH_LITERAL("")).value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx")).ReplaceExtension(PATH_LITERAL(".dat")).value());
        REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\xxx.dat")).value() ==
                Path(PATH_LITERAL("C:\\abc\\def\\xxx")).ReplaceExtension(PATH_LITERAL("dat")).value());
    }
}

TEST_CASE("Comparisons between paths", "[Path]")
{
    REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\test.txt")) == Path(PATH_LITERAL("C:\\abc\\def\\test.txt")));
    REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\test.txt")) != Path(PATH_LITERAL("C:\\abc\\def\\")));
    REQUIRE(Path(PATH_LITERAL("/home/kc")) == Path(PATH_LITERAL("/home/kc")));
    REQUIRE(Path(PATH_LITERAL("/home/kc")) != Path(PATH_LITERAL("/home/test")));
    REQUIRE_FALSE(Path(PATH_LITERAL("/home/kc")) < Path(PATH_LITERAL("/home/kc")));
    REQUIRE(Path(PATH_LITERAL("/home/kc")) < Path(PATH_LITERAL("/home/zen")));

    // Case-sensitivity
#if defined(OS_WIN)
    REQUIRE(Path(PATH_LITERAL("C:\\abc\\def\\test.txt")) == Path(PATH_LITERAL("c:\\ABC\\def\\TEST.TXT")));
#else
    REQUIRE_FALSE(Path(PATH_LITERAL("/home/kc")) == Path(PATH_LITERAL("/HOME/kc")));
#endif
}

}   // namespace kbase
