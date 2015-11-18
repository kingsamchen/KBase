/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_ENVIRONMENT_H_
#define KBASE_ENVIRONMENT_H_

#include <map>
#include <string>

namespace kbase {

using EnvTable = std::map<std::wstring, std::wstring>;

class Environment {
public:
    Environment() = delete;

    Environment(const Environment&) = delete;

    Environment(Environment&&) = delete;

    Environment& operator=(const Environment&) = delete;

    Environment& operator=(Environment&&) = delete;

    ~Environment() = delete;

    // Gets the value of an environment variable.
    // Returns an empty string if no such variable exists.
    static std::wstring GetVar(const wchar_t* name);

    // Returns true if an environment variable specified by the given |name| exists.
    // Returns false otherwise.
    static bool HasVar(const wchar_t* name);

    // Creates an environment variable with the given name and value.
    static void SetVar(const wchar_t* name, const std::wstring& value);

    // Removes an existing environment variable.
    // This function has no effect if the variable to be removed does not exist.
    static void RemoveVar(const wchar_t* name);

    // Gets an environment table corresponding to the environment block of
    // current process.
    static EnvTable CurrentEnvironmentTable();

    // Gets an environment block from an environment table.
    static std::wstring GetEnvironmentBlock(const EnvTable& env_table);
};

}   // namespace kbase

#endif  // KBASE_ENVIRONMENT_H_