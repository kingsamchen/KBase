/*
 @ 0xCCCCCCCC
*/

#include "gtest/gtest.h"

#include "kbase/base_path_provider.h"
#include "kbase/minidump.h"
#include "kbase/path_service.h"

namespace {

void Logging(int i)
{
    std::cout << "Logging input value: " << i << std::endl;
    auto dump_file_path = kbase::PathService::Get(kbase::DirTemp).AppendASCII("kbase_test.dmp");
    auto rv = kbase::CreateMiniDump(dump_file_path);
    EXPECT_TRUE(rv);
}

int Double(int i)
{
    Logging(i);
    return i * 2;
}

int Inc(int i)
{
    auto n = Double(i) + i;
    return n;
}

}   // namespace

namespace kbase {

void Foo(int i)
{
    Inc(i);;
}

TEST(MiniDumpTest, GenerateMinidump)
{
    Foo(123);
}

}   // namespace
