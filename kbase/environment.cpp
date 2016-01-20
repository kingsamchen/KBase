/*
 @ 0xCCCCCCCC
*/

#include "kbase/environment.h"

#include <Windows.h>

#include <vector>

#include "kbase/error_exception_util.h"
#include "kbase/string_piece.h"

namespace {

// not including null-terminator
const DWORD kEnvVarMaxSize = 32766;

void ParseEnvironmentBlock(const wchar_t* block_string, kbase::EnvTable* env_table)
{
    ENSURE(CHECK, block_string && *block_string != L'\0').Require();

    auto* cur = block_string;
    auto* field_begin = cur;
    kbase::WStringPiece key, value;

    while (*cur != L'\0' || *(cur - 1) != L'\0') {
        if (*cur == L'=') {
            key.set(field_begin, cur);
            field_begin = cur + 1;
        } else if (*cur == L'\0') {
            value.set(field_begin, cur);
            field_begin = cur + 1;
            env_table->emplace(std::make_pair(key.as_string(), value.as_string()));
        }

        ++cur;
    }
}

}   // namespace

namespace kbase {

// static
std::wstring Environment::GetVar(const wchar_t* name)
{
    DWORD required_size = GetEnvironmentVariableW(name, nullptr, 0);
    if (required_size == 0 &&
        LastError().error_code() == ERROR_ENVVAR_NOT_FOUND) {
        return std::wstring();
    }

    std::vector<wchar_t> buf(required_size);
    DWORD rv = GetEnvironmentVariableW(name, &buf[0], required_size);
    ENSURE(RAISE, rv != 0)(LastError()).Require("Failed to get environment variable");

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
    ENSURE(CHECK, value.size() <= kEnvVarMaxSize)(value.size()).Require();
    BOOL rv = SetEnvironmentVariableW(name, value.c_str());
    ENSURE(RAISE, rv != 0)(LastError()).Require("Failed to set environment variable");
}

// static
void Environment::RemoveVar(const wchar_t* name)
{
    SetEnvironmentVariableW(name, nullptr);
}

// static
EnvTable Environment::CurrentEnvironmentTable()
{
    auto env_block = GetEnvironmentStringsW();
    EnvTable current_env_table;
    ParseEnvironmentBlock(env_block, &current_env_table);
    FreeEnvironmentStringsW(env_block);

    return current_env_table;
}

// static
std::wstring Environment::GetEnvironmentBlock(const EnvTable& env_table)
{
    ENSURE(CHECK, !env_table.empty()).Require();

    std::wstring env_block;

    for (const auto& item : env_table) {
        env_block.append(item.first).append(1, L'=');
        env_block.append(item.second).append(1, L'\0');
    }

    env_block.append(1, L'\0');

    return env_block;
}

}   // namespace kbase