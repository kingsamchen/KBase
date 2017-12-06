/*
 @ 0xCCCCCCCC
*/

#include <map>
#include <vector>

#include "gtest/gtest.h"

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

class ErrorExceptionUtilTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        // Don't trigger check during test.
        AlwaysCheckFirstInDebug(false);
    }
};

TEST_F(ErrorExceptionUtilTest, EnsureMacro)
{
    std::vector<int> v {1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto foo = [&] {
        ENSURE(THROW, v.size() >= 0 && v.size() <= 5)(v.size()).Require();
    };

    EXPECT_THROW(foo(), EnsureFailure);
}

TEST_F(ErrorExceptionUtilTest, ThrowWithCustomizedException)
{
    std::vector<int> v;

    auto foo = [&v]() {
        ENSURE(THROW, !v.empty()).ThrowIn<YetAnotherException>().Require();
    };

    auto bar = [&v]() {
        ENSURE(THROW, !v.empty()).ThrowIn<YetAnotherException>()
                                 .Require("The vector can't be empty!");
    };

    EXPECT_THROW(foo(), YetAnotherException);
    EXPECT_THROW(bar(), YetAnotherException);
}

TEST_F(ErrorExceptionUtilTest, NoInstanceOnSuccess)
{
    bool flag = false;
    RunTracer run_tracer(flag);

    ENSURE(THROW, true)(run_tracer).Require("blahblah");

    EXPECT_FALSE(flag);
}

TEST_F(ErrorExceptionUtilTest, CaptureWideString)
{
    std::wstring ws = L"std::wstring";
    WStringView wsv = ws;

    ENSURE(THROW, true)(ws)(wsv).Require();
}

#if defined(OS_WIN)

TEST_F(ErrorExceptionUtilTest, AcquireLastError)
{
    SetLastError(128);
    LastError error;
    DWORD error_code = GetLastError();
    ASSERT_EQ(error.error_code(), error_code);
}

TEST_F(ErrorExceptionUtilTest, LastErrorMessage)
{
    ClearLastError();
    LastError error;
    DWORD error_code = GetLastError();
    ASSERT_EQ(0, error_code);
    EXPECT_STREQ(error.GetDescriptiveMessage().c_str(), code2message[error_code]);

    SetLastError(128);
    error = LastError();
    error_code = GetLastError();
    EXPECT_STREQ(error.GetDescriptiveMessage().c_str(), code2message[error_code]);

    // Error message that being stored in a single module.

    SetLastError(NERR_NetworkError);
    error = LastError();
    EXPECT_STREQ(error.GetDescriptiveMessage().c_str(), code2message[NERR_NetworkError]);
}

#endif

}   // namespace kbase
