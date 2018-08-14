/*
 @ 0xCCCCCCCC
*/

#include <map>
#include <vector>

#include "catch2/catch.hpp"

#include "kbase/error_exception_util.h"

#if defined(OS_WIN)
#include <lmerr.h>
#endif

namespace {

class YetAnotherException : public std::logic_error {
public:
    explicit YetAnotherException(const std::string& msg)
        : std::logic_error(msg.c_str())
    {}
};

struct RunTracer {
    explicit RunTracer(bool& flag)
        : ref(flag)
    {}

    void set() const
    {
        ref = true;
    }

    bool& ref;
};

std::ostream& operator<< (std::ostream& os, const RunTracer& lhs)
{
    lhs.set();
    os << "Run flag has been set!";
    return os;
}

#if defined(OS_WIN)

std::map<DWORD, const wchar_t*> code2message = {
    {0, L"The operation completed successfully."},
    {123, L"The filename, directory name, or volume label syntax is incorrect."},
    {128, L"There are no child processes to wait for."},
    {NERR_NetworkError, L"A general network error occurred."}
};

void ClearLastError()
{
    SetLastError(0);
}

#endif  // OS_WIN

}   // namespace

namespace kbase {

TEST_CASE("General usages of ENSURE macro", "[ErrorExceptionUtil]")
{
    // Suppress checking during tests.
    AlwaysCheckFirstInDebug(false);

    SECTION("using ENSURE macro")
    {
        std::vector<int> v {1, 2, 3, 4, 5, 6, 7, 8, 9};

        auto foo = [&] {
            ENSURE(THROW, v.size() >= 0 && v.size() <= 5)(v.size()).Require();
        };

        REQUIRE_THROWS_AS(foo(), EnsureFailure);
    }

    SECTION("we can throw customized exception when a conditino is violated")
    {
        std::vector<int> v;

        auto foo = [&v]() {
            ENSURE(THROW, !v.empty()).ThrowIn<YetAnotherException>().Require();
        };

        auto bar = [&v]() {
            ENSURE(THROW, !v.empty()).ThrowIn<YetAnotherException>()
                                     .Require("The vector can't be empty!");
        };

        REQUIRE_THROWS_AS(foo(), YetAnotherException);
        REQUIRE_THROWS_AS(bar(), YetAnotherException);
    }

    SECTION("no capture overhead on success")
    {
        bool flag = false;
        RunTracer run_tracer(flag);

        ENSURE(THROW, true)(run_tracer).Require("blahblah");

        REQUIRE_FALSE(flag);
    }

    SECTION("able to capture wide strings")
    {
        std::wstring ws = L"std::wstring";
        WStringView wsv = ws;

        ENSURE(THROW, true)(ws)(wsv).Require();
    }

    SECTION("able to capture enum types")
    {
        enum class E : unsigned int {
            A = 0xDEADBEEF,
        };

        E e = E::A;

        ENSURE(THROW, true)(e).Require();
    }
}

#if defined(OS_WIN)

TEST_CASE("Use LastError to embody GetLastError() on Windows")
{
    SECTION("acquire last error")
    {
        SetLastError(128);
        LastError error;
        DWORD error_code = GetLastError();
        REQUIRE(error.error_code() == error_code);
    }

    SECTION("automatically translate error code to human readable error message")
    {
        ClearLastError();
        LastError error;
        DWORD error_code = GetLastError();
        REQUIRE(0 == error_code);
        REQUIRE(error.GetDescriptiveMessage() == code2message[error_code]);

        SetLastError(128);
        error = LastError();
        error_code = GetLastError();
        REQUIRE(error.GetDescriptiveMessage() == code2message[error_code]);

        // Error message that being stored in a single module.

        SetLastError(NERR_NetworkError);
        error = LastError();
        REQUIRE(error.GetDescriptiveMessage() == code2message[NERR_NetworkError]);
    }
}

#endif

}   // namespace kbase
