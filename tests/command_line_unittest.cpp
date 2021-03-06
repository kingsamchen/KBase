/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include "kbase/command_line.h"

namespace kbase {

TEST_CASE("Constructing an instance", "[CommandLine]")
{
    SECTION("from given program path") {
#if defined(OS_WIN)
        Path path(L"C:\\windows\\system32\\calc.exe");
#elif defined(OS_POSIX)
        Path path("/usr/bin/calc");
#endif
        CommandLine cmdline(path);
        REQUIRE(cmdline.GetArgs().size() == 1);
        REQUIRE(cmdline.parameter_count() == 0);
        REQUIRE(cmdline.GetProgram() == path);
    }

    SECTION("from argc and argv") {
        const char* argv[] {
            "/usr/local/bin/test", "--test", "--dry-run=true", "1234"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        CommandLine cmdline(argc, argv);
        REQUIRE(cmdline.GetArgs().size() == argc);
        REQUIRE(cmdline.GetProgram() == Path::FromUTF8(argv[0]));
        REQUIRE(cmdline.parameter_count() == 1);
    }

    SECTION("from args") {
        CommandLine::ArgList args {
            "/usr/local/bin/test", "--test", "--dry-run=true", "1234"
        };

        CommandLine cmdline(args);
        REQUIRE(cmdline.GetArgs().size() == args.size());
        REQUIRE(cmdline.GetProgram() == Path::FromUTF8(args[0]));
        REQUIRE(cmdline.parameter_count() == 1);
    }
}

TEST_CASE("Init and use for current process", "[CommandLine]")
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

TEST_CASE("Get and set program part", "[CommandLine]")
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

TEST_CASE("Use of switches", "[CommandLine]")
{
    SECTION("test if a switch is given") {
        CommandLine::ArgList args {
            "/usr/bin/calc",
            "-re",
            "123",
            "--maxmize",
            "test.txt",
            "/silent"
        };

        CommandLine cmdline(args);
        REQUIRE(cmdline.GetProgram() == Path::FromUTF8("/usr/bin/calc"));

        CHECK(cmdline.HasSwitch("re"));
        CHECK(cmdline.HasSwitch("maxmize"));
        CHECK(cmdline.HasSwitch("silent"));
        CHECK_FALSE(cmdline.HasSwitch("test"));
    }

    SECTION("a switch can optionally have a value") {
        CommandLine::ArgList args {
            "/usr/bin/calc",
            "-a=1",
            "123",
            "--t=1024",
            "--maxmize",
            "test.txt",
            "--on-the-fly=True"
        };

        CommandLine cmdline(args);

        // Get switch values (in string), and use default value when switch not present.
        CHECK(cmdline.GetSwitchValue("a", "0") == std::string("1"));
        CHECK(cmdline.GetSwitchValue("ta", "none") == std::string("none"));

        // With data conversion
        CHECK(cmdline.GetSwitchValueAs<int>("t") == 1024);
        CHECK(cmdline.GetSwitchValueAs<bool>("on-the-fly", false));
        // Throws an exception when conversion failed.
        CHECK_THROWS_AS(cmdline.GetSwitchValueAs<bool>("maxmize"), CommandLineValueParseError);

        // Use default value & customized converter.
        auto cvt = [](const auto& v) { return static_cast<unsigned short>(std::stoi(v)); };
        CHECK(cmdline.GetSwitchValueAs<unsigned short>("port", 9876, cvt) == 9876);

        SECTION("try what exception message might be") {
            cmdline.AppendSwitch("seq", "asdf");
            try {
                IGNORE_RESULT(cmdline.GetSwitchValueAs<int>("seq"));
            } catch (const CommandLineValueParseError& ex) {
                printf("%s\n", ex.what());
            }
        }
    }

    SECTION("change switch prefix for outputing commandline string") {
        CommandLine::ArgList args {
            "/usr/bin/calc",
            "-test",
            "--maximize",
            "/env=dev"
        };

        CommandLine cmdline(args);

        SECTION("when using double-dash by default") {
            REQUIRE(cmdline.switch_prefix() == CommandLine::SwitchPrefix::DoubleDash);
            CHECK(cmdline.GetCommandLineStringWithoutProgram() == "--test --maximize --env=dev");
        }

        SECTION("when change to using dash") {
            cmdline.set_switch_prefix(CommandLine::SwitchPrefix::Dash);
            REQUIRE(cmdline.switch_prefix() == CommandLine::SwitchPrefix::Dash);
            CHECK(cmdline.GetCommandLineStringWithoutProgram() == "-test -maximize -env=dev");
        }

        SECTION("when change to using slash") {
            cmdline.set_switch_prefix(CommandLine::SwitchPrefix::Slash);
            REQUIRE(cmdline.switch_prefix() == CommandLine::SwitchPrefix::Slash);
            CHECK(cmdline.GetCommandLineStringWithoutProgram() == "/test /maximize /env=dev");
        }
    }
}

TEST_CASE("Use of parameters", "[CommandLine]")
{
    SECTION("obtain count of parameters") {
        CommandLine::ArgList args {
            "/usr/bin/calc",
            "-a=1",
            "--t=1024",
            "/maxmize",
        };

        CommandLine cmdline(args);
        CHECK(cmdline.parameter_count() == 0);

        cmdline.AppendParameter("test").AppendParameter("goose");
        CHECK(cmdline.parameter_count() == 2);
    }

    SECTION("obtain a parameter at given index") {
        CommandLine::ArgList args {
            "/usr/bin/calc",
            "-a=1",
            "123",
            "--t=1024",
            "1970/01/01",
            "/maxmize",
            "test.txt"
        };

        CommandLine cmdline(args);
        REQUIRE(cmdline.parameter_count() == 3);

        SECTION("access to parameter values") {
            CHECK(cmdline.GetParameter(0) == "123");
            CHECK(cmdline.GetParameter(1) == "1970/01/01");
            CHECK(cmdline.GetParameter(2) == "test.txt");

            SECTION("out of index access would cause an exception thrown") {
                CHECK_THROWS(cmdline.GetParameter(9));
            }
        }

        SECTION("access to parameter values with data conversions") {
            CHECK(cmdline.GetParameterAs<int>(0) == 123);
            CHECK(cmdline.GetParameterAs<Path>(2, [](const auto& v) { return Path::FromUTF8(v); }) ==
                  Path::FromUTF8("test.txt"));

            SECTION("cases that throw an exception") {
                // Data conversion failure.
                CHECK_THROWS_AS(cmdline.GetParameterAs<bool>(0), CommandLineValueParseError);

                // Out of index.
                CHECK_THROWS(cmdline.GetParameter(9));
            }
        }
    }
}

TEST_CASE("Get full commandline in string", "[CommandLine]")
{
    CommandLine::ArgList args {
#if defined(OS_WIN)
        "D:\\program files\\test.exe",
#elif defined(OS_POSIX)
        "/usr/bin/calc",
#endif
        "-a=1",
        "123",
        "--t=1024",
        "1970/01/01",
        "/maxmize",
        "test.txt"
    };

    CommandLine cmdline(args);
    REQUIRE(cmdline.switch_prefix() == CommandLine::SwitchPrefix::DoubleDash);
    auto cmdstr = cmdline.GetCommandLineString();
#if defined(OS_WIN)
    auto program = "\"" + args[0] + "\"";
#elif defined(OS_POSIX)
    auto program = args[0];
#endif
    auto expected = program + " --a=1 --t=1024 --maxmize 123 1970/01/01 test.txt";
    CHECK(cmdstr == expected);
}

TEST_CASE("Value converters", "[CommandLine]")
{
#define XSTR(x) std::to_string(x)

    SECTION("to int32_t") {
        auto cvt = internal::ValueConverter<int32_t>();

        // Normal decimal
        CHECK(INT32_MAX == cvt(XSTR(INT32_MAX)));
        CHECK(INT32_MIN == cvt(XSTR(INT32_MIN)));
        CHECK(0 == cvt("0"));
        CHECK(-1 == cvt("-1"));

        // Hex support
        CHECK(65535 == cvt("0xffff"));
        CHECK(65535 == cvt("0XFFFF"));

        // Out of range
        CHECK_THROWS_AS(cvt(XSTR(INT32_MAX + 1ll)), std::out_of_range);
        CHECK_THROWS_AS(cvt(XSTR(INT32_MIN - 1ll)), std::out_of_range);
    }

    SECTION("to uint32_t") {
        auto cvt = internal::ValueConverter<uint32_t>();

        CHECK(UINT32_MAX == cvt(XSTR(UINT32_MAX)));
        CHECK(INT32_MAX + 1u == cvt(XSTR(INT32_MAX + 1u)));
        CHECK(UINT32_MAX == cvt("-1"));
        CHECK(INT32_MAX + 1u == cvt(XSTR(INT32_MIN)));
        CHECK(0 == cvt("0x00"));

        CHECK_THROWS_AS(cvt(XSTR(UINT32_MAX + 1ull)), std::out_of_range);
    }

    SECTION("to int64_t") {
        auto cvt = internal::ValueConverter<int64_t>();

        // Normal decimal
        CHECK(INT64_MAX == cvt(XSTR(INT64_MAX)));
        CHECK(INT64_MIN == cvt(XSTR(INT64_MIN)));
        CHECK(0 == cvt("0"));
        CHECK(-1 == cvt("-1"));

        // Hex support
        CHECK(65535 == cvt("0xffff"));
        CHECK(65535 == cvt("0XFFFF"));

        // Out of range
        CHECK_THROWS_AS(cvt("9223372036854775808"), std::out_of_range);
        CHECK_THROWS_AS(cvt("-9223372036854775809"), std::out_of_range);
    }

    SECTION("to uint64_t") {
        auto cvt = internal::ValueConverter<uint64_t>();

        // Normal decimal
        CHECK(UINT64_MAX == cvt(XSTR(UINT64_MAX)));
        CHECK(0 == cvt("0"));
        CHECK(UINT64_MAX == cvt("-1"));

        // Hex support
        CHECK(65535 == cvt("0xffff"));
        CHECK(65535 == cvt("0XFFFF"));

        // Out of range
        CHECK_THROWS_AS(cvt("170141183460469231731687303715884105727"), std::out_of_range);
    }

    SECTION("to int16_t") {
        auto cvt = internal::ValueConverter<int16_t>();

        CHECK(INT16_MAX == cvt(XSTR(INT16_MAX)));
        CHECK(INT16_MIN == cvt(XSTR(INT16_MIN)));
        CHECK(0 == cvt("0"));
        CHECK(-1 == cvt("-1"));

        CHECK(255 == cvt("0xff"));
        CHECK(255 == cvt("0XFF"));

        CHECK_THROWS_AS(cvt(XSTR(INT16_MAX + 1)), std::out_of_range);
        CHECK_THROWS_AS(cvt(XSTR(INT16_MIN - 1)), std::out_of_range);
    }

    SECTION("to uint16_t") {
        auto cvt = internal::ValueConverter<uint16_t>();

        CHECK(UINT16_MAX == cvt(XSTR(UINT16_MAX)));
        CHECK(0 == cvt("0"));
        CHECK(UINT16_MAX == cvt("-1"));
        CHECK(INT16_MAX + static_cast<uint16_t>(1) == cvt(XSTR(INT16_MIN)));

        CHECK(255 == cvt("0xff"));
        CHECK(255 == cvt("0XFF"));

        CHECK_THROWS_AS(cvt(XSTR(UINT16_MAX + 1)), std::out_of_range);
        CHECK_THROWS_AS(cvt(XSTR(INT16_MIN - 1)), std::out_of_range);
    }

#undef XSTR
}

}   // namespace kbase
