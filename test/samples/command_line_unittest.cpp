/*
 @ 0xCCCCCCCC
*/

#include "gtest/gtest.h"

#include "kbase/command_line.h"

TEST(CommandLineTest, Ctors)
{
    {
        kbase::Path path(L"C:\\windows\\system32\\calc.exe");
        kbase::CommandLine cmdline(path);
        auto program = cmdline.GetProgram();
        std::cout << program.AsASCII() << std::endl;
        EXPECT_EQ(program, path);

        int argc = 3;
        const wchar_t* argv[] {L"C:\\abc.exe", L"-topmost=true", L"123"};
        kbase::CommandLine dummy_cmdline(argc, argv);
    }

    // copy-semantics
    {
        const kbase::CommandLine& cmdline = kbase::CommandLine::ForCurrentProcess();
        kbase::CommandLine new_copied_cmdline(cmdline);
        kbase::CommandLine copy_op_cmdline(L"C:\\windows\\system32\\notepad.exe");
        copy_op_cmdline = new_copied_cmdline;
        EXPECT_EQ(new_copied_cmdline.GetArgv(), copy_op_cmdline.GetArgv());
    }

    // move-semantics
    {
        const kbase::CommandLine& cmdline = kbase::CommandLine::ForCurrentProcess();
        kbase::CommandLine new_copied_cmdline(cmdline);
        kbase::CommandLine new_moved_cmdline(std::move(new_copied_cmdline));
        EXPECT_EQ(new_moved_cmdline.GetArgv(), cmdline.GetArgv());
    }
}

TEST(CommandLineTest, CurrentProcessCommandLine)
{
    const kbase::CommandLine& cmdline = kbase::CommandLine::ForCurrentProcess();
    auto argv = cmdline.GetArgv();
    for (const auto& arg : argv) {
        std::wcout << arg << std::endl;
    }
    EXPECT_FALSE(argv.empty());
}

TEST(CommandLineTest, GetOrSetProgram)
{
    kbase::Path path(L"C:\\windows\\system32\\calc.exe");
    kbase::CommandLine cmdline(path);
    EXPECT_EQ(cmdline.GetProgram().value(), path.value());

    kbase::Path path_with_space(L"  C:\\windows\\system32\\calc.exe ");
    cmdline.SetProgram(path_with_space);
    EXPECT_EQ(cmdline.GetProgram(), path);

    kbase::Path path_with_tabs(L"\tC:\\windows\\system32\\calc.exe\t\t");
    cmdline.SetProgram(path_with_tabs);
    EXPECT_EQ(cmdline.GetProgram(), path);

    kbase::Path path_with_hybrid(L" \tC:\\windows\\system32\\calc.exe \t ");
    cmdline.SetProgram(path_with_hybrid);
    EXPECT_EQ(cmdline.GetProgram(), path);
}

TEST(CommandLineTest, DefaultSwithPrefix)
{
    kbase::Path path(L"C:\\windows\\system32\\calc.exe");
    kbase::CommandLine cmdline(path);
    EXPECT_EQ(cmdline.GetDefaultSwitchPrefix(), kbase::CommandLine::PREFIX_DASH);

    cmdline.SetDefaultSwitchPrefix(kbase::CommandLine::PREFIX_SLASH);
    EXPECT_NE(cmdline.GetDefaultSwitchPrefix(), kbase::CommandLine::PREFIX_DASH);
    EXPECT_EQ(cmdline.GetDefaultSwitchPrefix(), kbase::CommandLine::PREFIX_SLASH);
}

TEST(CommandLineTest, HasSwitch)
{
    std::wstring cmdline_str = L"C:\\windows\\system32\\notepad.exe -r --maxmize D:\\test.txt";
    kbase::CommandLine cmdline(cmdline_str);
    ASSERT_EQ(cmdline.GetProgram(), kbase::Path(L"C:\\windows\\system32\\notepad.exe"));

    EXPECT_TRUE(cmdline.HasSwitch(L"r"));
    EXPECT_TRUE(cmdline.HasSwitch(L"maxmize"));
    EXPECT_FALSE(cmdline.HasSwitch(L"path"));
}

TEST(CommandLineTest, QuerySwitchValue)
{
    kbase::CommandLine cmdline(L"C:\\windows\\system32\\notepad.exe -a=1 --maxmize --t=1024 D:\\test.txt");

    std::wstring value;
    bool rv = cmdline.GetSwitchValue(L"a", &value);
    EXPECT_TRUE(rv);
    EXPECT_EQ(value, L"1");

    rv = cmdline.GetSwitchValue(L"maxmize", &value);
    EXPECT_TRUE(rv);
    EXPECT_TRUE(value.empty());

    rv = cmdline.GetSwitchValue(L"ta", &value);
    EXPECT_FALSE(rv);
}

TEST(CommandLineTest, GetParameters)
{
    {
        kbase::CommandLine cmdline(L"C:\\abc.exe -a=1 -t=123 D:\\test.txt 3600 2014/11/25");
        auto params = cmdline.GetParameters();
        std::vector<std::wstring> p {L"D:\\test.txt", L"3600", L"2014/11/25"};
        EXPECT_EQ(params, p);
    }

    {
        kbase::CommandLine cmdline(L"C:\\abc.exe -a=1 -t=123");
        auto params = cmdline.GetParameters();
        std::vector<std::wstring> empty;
        EXPECT_EQ(params, empty);
    }
}

TEST(CommandLineTest, GetArgv)
{
    kbase::CommandLine::ArgList argv {L"D:\\test.exe", L"-a=1", L"-t=1024", L"-warmup", L"D:\\test.txt", L"2014-11-25"};
    {
    kbase::CommandLine cmdline(argv);
    auto parsed_argv = cmdline.GetArgv();
    EXPECT_EQ(parsed_argv, argv);
    }

    {
    kbase::CommandLine cmdline(L"D:\\test.exe --a=1 /t=1024 --warmup D:\\test.txt 2014-11-25");
    auto parsed_argv = cmdline.GetArgv();
    EXPECT_EQ(parsed_argv, argv);
    }
}

TEST(CommandLineTest, GerArgvWithoutProgramStr)
{
    kbase::CommandLine cmdline(L"D:\\test.exe --a=1 /t=1024 --warmup D:\\test.txt 2014-11-25");
    auto argv_str = cmdline.GetArgvStringWithoutProgram();
    EXPECT_EQ(argv_str, L"-a=1 -t=1024 -warmup D:\\test.txt 2014-11-25");
}

TEST(CommandLineTest, GetCommandLineString)
{
    kbase::CommandLine::ArgList argv{L"D:\\program files\\test.exe", L"-a=1", L"-t=1024", L"-warmup", L"D:\\test.txt", L"2014-11-25"};
    kbase::CommandLine cmdline(argv);
    auto cmdline_str = cmdline.GetCommandLineString();
    std::wcout << cmdline_str << std::endl;
    EXPECT_EQ(cmdline_str, L"\"D:\\program files\\test.exe\" -a=1 -t=1024 -warmup D:\\test.txt 2014-11-25");
}