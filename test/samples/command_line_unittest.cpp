/*
 @ 0xCCCCCCCC
*/

#include "gtest/gtest.h"

#include "kbase/basic_types.h"
#include "kbase/command_line.h"
#include "kbase/string_encoding_conversions.h"

namespace kbase {

TEST(CommandLineTest, Ctors)
{
#if defined(OS_WIN)
        Path path(L"C:\\windows\\system32\\calc.exe");
#else
        Path path("/usr/bin/calc");
#endif
        CommandLine cmdline(path);
        auto program = cmdline.GetProgram();
        EXPECT_EQ(program, path);

        int argc = 3;
        const CommandLine::CharType* argv[] {
            CMDLINE_LITERAL("/home/abc"), CMDLINE_LITERAL("--topmost=true"), CMDLINE_LITERAL("123")
        };

        CommandLine dummy_cmdline(argc, argv);
        UNUSED_VAR(dummy_cmdline);

#if defined(OS_WIN)
    std::wstring cmdline_str = L"C:\\windows\\system32\\notepad.exe -r --maxmize D:\\test.txt";
    CommandLine cmdline_from_str(cmdline_str);
    EXPECT_EQ(cmdline_from_str.GetProgram(), Path(L"C:\\windows\\system32\\notepad.exe"));
#endif
}

#if defined(OS_WIN)
TEST(CommandLineTest, CurrentProcessCommandLine)
{
    CommandLine::Init(0, nullptr);
    const CommandLine& cmdline = kbase::CommandLine::ForCurrentProcess();
    auto argv = cmdline.GetArgs();
    for (const auto& arg : argv) {
        std::wcout << arg << std::endl;
    }

    EXPECT_FALSE(argv.empty());
}
#endif

TEST(CommandLineTest, GetOrSetProgram)
{
    Path path(PATH_LITERAL("/usr/bin/calc"));
    CommandLine cmdline(path);
    EXPECT_EQ(cmdline.GetProgram().value(), path.value());

    Path path_with_space(PATH_LITERAL("  /usr/bin/calc "));
    cmdline.SetProgram(path_with_space);
    EXPECT_EQ(cmdline.GetProgram(), path);

    Path path_with_tabs(PATH_LITERAL("\t/usr/bin/calc\t\t"));
    cmdline.SetProgram(path_with_tabs);
    EXPECT_EQ(cmdline.GetProgram(), path);

    Path path_with_mixed(PATH_LITERAL(" \t/usr/bin/calc \t "));
    cmdline.SetProgram(path_with_mixed);
    EXPECT_EQ(cmdline.GetProgram(), path);
}

TEST(CommandLineTest, SwithPrefix)
{
    Path path(PATH_LITERAL("/usr/bin/calc"));
    CommandLine cmdline(path);
    cmdline.AppendSwitch(CMDLINE_LITERAL("test"));
    EXPECT_EQ(cmdline.switch_prefix(), CommandLine::PrefixDoubleDash);

    cmdline.set_switch_prefix(CommandLine::PrefixSlash);
    EXPECT_NE(cmdline.switch_prefix(), CommandLine::PrefixDoubleDash);
    EXPECT_EQ(cmdline.switch_prefix(), CommandLine::PrefixSlash);
}

TEST(CommandLineTest, HasSwitch)
{
    CommandLine::ArgList args {
        CMDLINE_LITERAL("/usr/bin/calc"),
        CMDLINE_LITERAL("-r"),
        CMDLINE_LITERAL("123"),
        CMDLINE_LITERAL("--maxmize"),
        CMDLINE_LITERAL("test.txt")
    };

    CommandLine cmdline(args);
    ASSERT_EQ(cmdline.GetProgram(), Path(PATH_LITERAL("/usr/bin/calc")));

    EXPECT_TRUE(cmdline.HasSwitch(CMDLINE_LITERAL("r")));
    EXPECT_TRUE(cmdline.HasSwitch(CMDLINE_LITERAL("maxmize")));
    EXPECT_FALSE(cmdline.HasSwitch(CMDLINE_LITERAL("path")));
}

TEST(CommandLineTest, QuerySwitchValue)
{
    CommandLine::ArgList args {
        CMDLINE_LITERAL("/usr/bin/calc"),
        CMDLINE_LITERAL("-a=1"),
        CMDLINE_LITERAL("123"),
        CMDLINE_LITERAL("--t=1024"),
        CMDLINE_LITERAL("--maxmize"),
        CMDLINE_LITERAL("test.txt")
    };

    CommandLine cmdline(args);

    std::string value;
    bool rv = cmdline.GetSwitchValueASCII(CMDLINE_LITERAL("a"), value);
    EXPECT_TRUE(rv);
    EXPECT_EQ(value, std::string("1"));

    rv = cmdline.GetSwitchValueASCII(CMDLINE_LITERAL("maxmize"), value);
    EXPECT_TRUE(rv);
    EXPECT_TRUE(value.empty());

    rv = cmdline.GetSwitchValueASCII(CMDLINE_LITERAL("ta"), value);
    EXPECT_FALSE(rv);
}

TEST(CommandLineTest, GetParameters)
{
    {
        CommandLine::ArgList args {
            CMDLINE_LITERAL("/usr/bin/calc"),
            CMDLINE_LITERAL("-a=1"),
            CMDLINE_LITERAL("123"),
            CMDLINE_LITERAL("--t=1024"),
            CMDLINE_LITERAL("1970/01/01"),
            CMDLINE_LITERAL("--maxmize"),
            CMDLINE_LITERAL("test.txt")
        };

        CommandLine cmdline(args);
        auto params = cmdline.GetParameters();
        CommandLine::ArgList p {CMDLINE_LITERAL("123"), CMDLINE_LITERAL("1970/01/01"),
                                CMDLINE_LITERAL("test.txt")};
        EXPECT_EQ(params, p);
    }

    {
        CommandLine cmdline(CommandLine::ArgList{CMDLINE_LITERAL("/usr/bin/calc"),
            CMDLINE_LITERAL("-a=1"), CMDLINE_LITERAL("--t=1024")});
        auto params = cmdline.GetParameters();
        CommandLine::ArgList empty;
        EXPECT_EQ(params, empty);
    }
}

TEST(CommandLineTest, GerArgsWithoutProgramStr)
{
    CommandLine::ArgList args {
        CMDLINE_LITERAL("/usr/bin/calc"),
        CMDLINE_LITERAL("-a=1"),
        CMDLINE_LITERAL("123"),
        CMDLINE_LITERAL("--t=1024"),
        CMDLINE_LITERAL("1970/01/01"),
        CMDLINE_LITERAL("/maxmize"),
        CMDLINE_LITERAL("test.txt")
    };

    CommandLine cmdline(args);

    auto args_str = cmdline.GetArgsStringWithoutProgram();
    EXPECT_EQ(args_str,
              CommandLine::StringType(CMDLINE_LITERAL(
                "--a=1 --t=1024 --maxmize 123 1970/01/01 test.txt")));

    cmdline.set_switch_prefix(CommandLine::PrefixSlash);
    args_str = cmdline.GetArgsStringWithoutProgram();
    EXPECT_EQ(args_str,
              CommandLine::StringType(CMDLINE_LITERAL(
                "/a=1 /t=1024 /maxmize 123 1970/01/01 test.txt")));
}

TEST(CommandLineTest, GetCommandLineString)
{
    CommandLine::ArgList args {
#if defined(OS_WIN)
        CMDLINE_LITERAL("D:\\program files\\test.exe"),
#else
        CMDLINE_LITERAL("/usr/bin/calc"),
#endif
        CMDLINE_LITERAL("-a=1"),
        CMDLINE_LITERAL("123"),
        CMDLINE_LITERAL("--t=1024"),
        CMDLINE_LITERAL("1970/01/01"),
        CMDLINE_LITERAL("/maxmize"),
        CMDLINE_LITERAL("test.txt")
    };

    CommandLine cmdline(args);

#if defined(OS_WIN)
    auto cmdline_str = WideToUTF8(cmdline.GetCommandLineString());
#else
    auto cmdline_str = cmdline.GetCommandLineString();
#endif

    std::cout << cmdline_str << std::endl;

#if defined(OS_WIN)
    EXPECT_EQ(cmdline_str,
              "\"D:\\program files\\test.exe\" --a=1 --t=1024 --maxmize 123 1970/01/01 test.txt");
#else
    EXPECT_EQ(cmdline_str,
              "/usr/bin/calc --a=1 --t=1024 --maxmize 123 1970/01/01 test.txt");
#endif
}

TEST(CommandLineTest, CopyAndMove)
{
    // copy-semantics
    {
        CommandLine::ArgList args {
            CMDLINE_LITERAL("/home/abc"), CMDLINE_LITERAL("--topmost=true"), CMDLINE_LITERAL("123")
        };

        CommandLine cmdline(args);

        kbase::CommandLine new_copied_cmdline(cmdline);
        kbase::CommandLine copy_op_cmdline(Path(PATH_LITERAL("C:\\windows\\notepad.exe")));
        EXPECT_NE(new_copied_cmdline.GetCommandLineString(),
                  copy_op_cmdline.GetCommandLineString());

        copy_op_cmdline = new_copied_cmdline;
        EXPECT_EQ(new_copied_cmdline.GetCommandLineString(),
                  copy_op_cmdline.GetCommandLineString());
    }

    // move-semantics
    {
        CommandLine::ArgList args {
            CMDLINE_LITERAL("/home/abc"), CMDLINE_LITERAL("--topmost=true"), CMDLINE_LITERAL("123")
        };

        CommandLine cmdline(args);
        CommandLine another(std::move(cmdline));
        EXPECT_EQ(CommandLine::StringType(CMDLINE_LITERAL("--topmost=true 123")),
                  another.GetArgsStringWithoutProgram());
    }
}

}   // namespace kbase
