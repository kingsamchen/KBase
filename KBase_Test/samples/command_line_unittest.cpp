/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\command_line.h"

TEST(CommandLineTest, Ctors)
{
    kbase::FilePath path(L"C:\\windows\\system32\\calc.exe");
    kbase::CommandLine cmdline(path);
    auto program = cmdline.GetProgram();
    std::cout << program.AsASCII() << std::endl;
    EXPECT_EQ(program, path);
}

TEST(CommandLineTest, GetOrSetProgram)
{
    kbase::FilePath path(L"C:\\windows\\system32\\calc.exe");
    kbase::CommandLine cmdline(path);
    EXPECT_EQ(cmdline.GetProgram().value(), path.value());

    kbase::FilePath path_with_space(L"  C:\\windows\\system32\\calc.exe ");
    cmdline.SetProgram(path_with_space);
    EXPECT_EQ(cmdline.GetProgram(), path);

    kbase::FilePath path_with_tabs(L"\tC:\\windows\\system32\\calc.exe\t\t");
    cmdline.SetProgram(path_with_tabs);
    EXPECT_EQ(cmdline.GetProgram(), path);

    kbase::FilePath path_with_hybrid(L" \tC:\\windows\\system32\\calc.exe \t ");
    cmdline.SetProgram(path_with_hybrid);
    EXPECT_EQ(cmdline.GetProgram(), path);
}

TEST(CommandLineTest, DefaultSwithPrefix)
{
    kbase::FilePath path(L"C:\\windows\\system32\\calc.exe");
    kbase::CommandLine cmdline(path);
    EXPECT_EQ(cmdline.GetDefaultSwitchPrefix(), kbase::CommandLine::PREFIX_DASH);

    cmdline.SetDefaultSwitchPrefix(kbase::CommandLine::PREFIX_SLASH);
    EXPECT_NE(cmdline.GetDefaultSwitchPrefix(), kbase::CommandLine::PREFIX_DASH);
    EXPECT_EQ(cmdline.GetDefaultSwitchPrefix(), kbase::CommandLine::PREFIX_SLASH);
}