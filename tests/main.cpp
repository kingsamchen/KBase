/*
 @ 0xCCCCCCCC
*/

#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

#include "kbase/error_exception_util.h"

int main(int argc, char* argv[])
{
    // In case trigger debug-break when we need to throw an exception.
    kbase::AlwaysCheckFirstInDebug(false);
    int result = Catch::Session().run(argc, argv);
    return result;
}
