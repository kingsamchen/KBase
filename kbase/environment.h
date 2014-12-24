/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_ENVIRONMENT_H_
#define KBASE_ENVIRONMENT_H_

#include <string>

namespace kbase {

class Environment {
public:
    Environment() = delete;

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
};

}   // namespace kbase

#endif  // KBASE_ENVIRONMENT_H_