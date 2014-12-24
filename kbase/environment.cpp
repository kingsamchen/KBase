/*
 @ Kingsley Chen
*/

#include "kbase\environment.h"

#include <Windows.h>

#include <vector>

#include "kbase\error_exception_util.h"

namespace {

// not including null-terminator
const DWORD kEnvVarMaxSize = 32766;

}   // namespace

namespace kbase {

// static
std::wstring Environment::GetVar(const wchar_t* name)
{
    DWORD required_size = GetEnvironmentVariableW(name, nullptr, 0);
    if (required_size == 0 &&
        LastError().last_error_code() == ERROR_ENVVAR_NOT_FOUND) {
        return std::wstring();
    }
    
    std::vector<wchar_t> buf(required_size);
    DWORD rv = GetEnvironmentVariableW(name, &buf[0], required_size);
    ThrowLastErrorIf(!rv, "failed to get environment variable");

    return std::wstring(buf.data(), rv);
}

// static
bool Environment::HasVar(const wchar_t* name)
{
    DWORD required_size = GetEnvironmentVariableW(name, nullptr, 0);
    return required_size != 0;
}

// static
void Environment::SetVar(const wchar_t* name, const std::wstring& value)
{
    ENSURE(value.size() <= kEnvVarMaxSize)(value.size()).raise();
    BOOL rv = SetEnvironmentVariableW(name, value.c_str());
    ThrowLastErrorIf(!rv, "failed to set environment variable");
}

// static
void Environment::RemoveVar(const wchar_t* name)
{
    SetEnvironmentVariableW(name, nullptr);
}

}   // namespace kbase