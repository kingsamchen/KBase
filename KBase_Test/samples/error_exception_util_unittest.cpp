/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"

#include "windows.h"

#include <map>
#include <vector>

#include "gtest\gtest.h"
#include "kbase\error_exception_util.h"

using namespace kbase;

class ErrorExceptionUtilTest : public ::testing::Test {

};

namespace {

std::map<DWORD, const wchar_t*> code2message = {
    {123, L"The filename, directory name, or volume label syntax is incorrect."},
    {128, L"There are no child processes to wait for."},
};

void ErrorEmitter(DWORD error_code)
{
    SetLastError(error_code);
}

void ClearLastError()
{
    SetLastError(0);
}

void EnsureMacroTestIntermediate(const std::vector<int>& vec)
{
    // Must explicitly call the function to throw exception.
    ENSURE(vec.size() >= 0 && vec.size() <= 5)(vec.size()).raise();
}

}   // namespace

TEST_F(ErrorExceptionUtilTest, LastError_Acquire)
{
    ErrorEmitter(123);
    ASSERT_EQ(LastError().error_code(), GetLastError());

    ErrorEmitter(128);
    LastError error;
    DWORD error_code = GetLastError();
    ASSERT_NE(error_code, 123);
    ASSERT_EQ(error.error_code(), error_code);
}

TEST_F(ErrorExceptionUtilTest, LastError_Message)
{
    ErrorEmitter(123);
    LastError error;
    DWORD error_code = GetLastError();
    EXPECT_STREQ(error.GetDescriptiveMessage().c_str(), code2message[error_code]);

    ErrorEmitter(128);
    error = LastError();
    error_code = GetLastError();
    EXPECT_STREQ(error.GetDescriptiveMessage().c_str(), code2message[error_code]);

    ErrorEmitter(666);
    error = LastError();
    EXPECT_TRUE(error.GetDescriptiveMessage().empty());
}

TEST_F(ErrorExceptionUtilTest, ThrowLastError)
{
    ClearLastError();

    EXPECT_NO_THROW(ThrowLastErrorIf(GetLastError() != 0, "the last error should be 0"));

    ErrorEmitter(64);
    EXPECT_THROW(ThrowLastErrorIf(GetLastError() != ERROR_SUCCESS, ""), Win32Exception);
}

TEST_F(ErrorExceptionUtilTest, EnsureMacro)
{
    std::vector<int> v {1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_ANY_THROW(EnsureMacroTestIntermediate(v));
}