/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include <iostream>

#include "kbase/command_line.h"
#include "kbase/string_encoding_conversions.h"

namespace kbase {

TEST_CASE("Construction and acquire instance", "[CommandLine]")
{
    SECTION("instances can be directly constructed")
    {
#if defined(OS_WIN)
        Path path(L"C:\\windows\\system32\\calc.exe");
#else
        Path path("/usr/bin/calc");
#endif
        CommandLine cmdline(path);
        auto program = cmdline.GetProgram();
        REQUIRE(program == path);

        int argc = 3;
        const CommandLine::CharType* argv[]{
        CMDLINE_LITERAL("/home/abc"), CMDLINE_LITERAL("--topmost=true"), CMDLINE_LITERAL("123")
        };

        CommandLine dummy_cmdline(argc, argv);
        UNUSED_VAR(dummy_cmdline);

#if defined(OS_WIN)
        std::wstring cmdline_str = L"C:\\windows\\system32\\notepad.exe -r --maxmize D:\\test.txt";
        CommandLine cmdline_from_str(cmdline_str);
        REQUIRE(cmdline_from_str.GetProgram() == Path(L"C:\\windows\\system32\\notepad.exe"));
#endif
    }

    SECTION("one can use ForCurrrentProcess to acquire the instance for the process")
    {
#if defined(OS_WIN)
        CommandLine::Init(0, nullptr);
#else
        const char* args[] {
            "Test",
            "a1",
            "b2",
            "c3",
        };

        CommandLine::Init(4, args);
#endif
        const CommandLine& cmdline = kbase::CommandLine::ForCurrentProcess();
        auto argv = cmdline.GetArgs();
        REQUIRE_FALSE(argv.empty());
    }

    SECTION("CommandLine is copyable")
    {
        CommandLine::ArgList args{
            CMDLINE_LITERAL("/home/abc"), CMDLINE_LITERAL("--topmost=true"), CMDLINE_LITERAL("123")
        };

        CommandLine cmdline(args);

        kbase::CommandLine new_copied_cmdline(cmdline);
        kbase::CommandLine copy_op_cmdline(Path(PATH_LITERAL("C:\\windows\\notepad.exe")));
        REQUIRE(new_copied_cmdline.GetCommandLineString() !=
            copy_op_cmdline.GetCommandLineString());

        copy_op_cmdline = new_copied_cmdline;
        REQUIRE(new_copied_cmdline.GetCommandLineString() ==
            copy_op_cmdline.GetCommandLineString());
    }

    SECTION("CommandLine is movable")
    {
        CommandLine::ArgList args{
            CMDLINE_LITERAL("/home/abc"), CMDLINE_LITERAL("--topmost=true"), CMDLINE_LITERAL("123")
        };

        CommandLine cmdline(args);
        CommandLine another(std::move(cmdline));
        REQUIRE(CommandLine::StringType(CMDLINE_LITERAL("--topmost=true 123")) ==
                another.GetArgsStringWithoutProgram());
    }
}

TEST_CASE("We can query and set arguments and/or switches", "[CommandLine]")
{
    SECTION("get or set program part")
    {
        Path path(PATH_LITERAL("/usr/bin/calc"));
        CommandLine cmdline(path);
        REQUIRE(cmdline.GetProgram().value() == path.value());

        Path path_with_space(PATH_LITERAL("  /usr/bin/calc "));
        cmdline.SetProgram(path_with_space);
        REQUIRE(cmdline.GetProgram() == path);

        Path path_with_tabs(PATH_LITERAL("\t/usr/bin/calc\t\t"));
        cmdline.SetProgram(path_with_tabs);
        REQUIRE(cmdline.GetProgram() == path);

        Path path_with_mixed(PATH_LITERAL(" \t/usr/bin/calc \t "));
        cmdline.SetProgram(path_with_mixed);
        REQUIRE(cmdline.GetProgram() == path);
    }

    SECTION("the switch prefix can be changed for an instance")
    {
        Path path(PATH_LITERAL("/usr/bin/calc"));
        CommandLine cmdline(path);
        cmdline.AppendSwitch(CMDLINE_LITERAL("test"));
        REQUIRE(cmdline.switch_prefix() == CommandLine::PrefixDoubleDash);

        cmdline.set_switch_prefix(CommandLine::PrefixSlash);
        REQUIRE(cmdline.switch_prefix() != CommandLine::PrefixDoubleDash);
        REQUIRE(cmdline.switch_prefix() == CommandLine::PrefixSlash);
    }

    SECTION("test if a switch is given")
    {
        CommandLine::ArgList args {
            CMDLINE_LITERAL("/usr/bin/calc"),
            CMDLINE_LITERAL("-r"),
            CMDLINE_LITERAL("123"),
            CMDLINE_LITERAL("--maxmize"),
            CMDLINE_LITERAL("test.txt")
        };

        CommandLine cmdline(args);
        REQUIRE(cmdline.GetProgram() == Path(PATH_LITERAL("/usr/bin/calc")));

        REQUIRE(cmdline.HasSwitch(CMDLINE_LITERAL("r")));
        REQUIRE(cmdline.HasSwitch(CMDLINE_LITERAL("maxmize")));
        REQUIRE_FALSE(cmdline.HasSwitch(CMDLINE_LITERAL("path")));
    }

    SECTION("a switch can associate with a value")
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
        REQUIRE(cmdline.GetSwitchValueASCII(CMDLINE_LITERAL("a"), value));
        REQUIRE(value == std::string("1"));

        REQUIRE(cmdline.GetSwitchValueASCII(CMDLINE_LITERAL("maxmize"), value));
        REQUIRE(value.empty());
        REQUIRE_FALSE(cmdline.GetSwitchValueASCII(CMDLINE_LITERAL("ta"), value));
    }

    SECTION("parameters are those only values")
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
            REQUIRE(params == p);
        }

        {
            CommandLine cmdline(CommandLine::ArgList{CMDLINE_LITERAL("/usr/bin/calc"),
                CMDLINE_LITERAL("-a=1"), CMDLINE_LITERAL("--t=1024")});
            auto params = cmdline.GetParameters();
            CommandLine::ArgList empty;
            REQUIRE(params == empty);
        }
    }

    SECTION("we can obtain args in string without program path")
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
        REQUIRE(args_str == CommandLine::StringType(
            CMDLINE_LITERAL("--a=1 --t=1024 --maxmize 123 1970/01/01 test.txt")));

        cmdline.set_switch_prefix(CommandLine::PrefixSlash);
        args_str = cmdline.GetArgsStringWithoutProgram();
        REQUIRE(args_str == CommandLine::StringType(
            CMDLINE_LITERAL("/a=1 /t=1024 /maxmize 123 1970/01/01 test.txt")));
    }

    SECTION("we can obtain full args string in complete")
    {
        CommandLine::ArgList args{
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
        REQUIRE(cmdline_str ==
            "\"D:\\program files\\test.exe\" --a=1 --t=1024 --maxmize 123 1970/01/01 test.txt");
#else
        REQUIRE(cmdline_str == "/usr/bin/calc --a=1 --t=1024 --maxmize 123 1970/01/01 test.txt");
#endif
    }
}

}   // namespace kbase
