
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

    std::wstring GetVar(const wchar_t* name);

    bool HasVar(const wchar_t* name);

    void SetVar(const wchar_t* name, const std::wstring& value);

    void RemoveVar(const wchar_t* name);
};

}   // namespace kbase

#endif  // KBASE_ENVIRONMENT_H_