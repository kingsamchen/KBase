/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include "kbase/base_path_provider.h"
#include "kbase/error_exception_util.h"
#include "kbase/guid.h"
#include "kbase/minidump.h"
#include "kbase/path_service.h"

namespace {

void Logging(int i)
{
    UNUSED_VAR(i);
    std::string filename = kbase::GenerateGUID() + "_kbase_test.dmp";
    auto dump_file_path = kbase::PathService::Get(kbase::DirTemp).AppendASCII(filename);
    auto rv = kbase::CreateMiniDump(dump_file_path);
    REQUIRE(rv);
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

TEST_CASE("Show general usage of minidump", "[MiniDump]")
{
    Foo(123);
}

}   // namespace
