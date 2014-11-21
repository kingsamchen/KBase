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