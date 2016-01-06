/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_ERROR_EXCEPTION_UTIL_H_
#define KBASE_ERROR_EXCEPTION_UTIL_H_

#include <cassert>
#include <sstream>
#include <string>
#include <stdexcept>

#include "kbase/basic_macros.h"
#include "kbase/string_format.h"
#include "kbase/sys_string_encoding_conversions.h"

namespace kbase {

enum class EnsureAction : int {
    CHECK,
    RAISE,
    RAISE_WITH_DUMP
};

// The action `CHECK` is performed only in debug mode.
// Besides, we also need to make the CHECK-call cause no runtime penalty,
// when in non-debug mode.
#if defined(NDEBUG)
#define ACTION_IS_ON(action) (kbase::EnsureAction::##action != kbase::EnsureAction::CHECK)
#else
#define ACTION_IS_ON(action) true
#endif

#define GUARANTOR_A(x) GUARANTOR_OP(x, B)
#define GUARANTOR_B(x) GUARANTOR_OP(x, A)
#define GUARANTOR_OP(x, next) \
    GUARANTOR_A.CaptureValue(#x, (x)).GUARANTOR_##next

#define MAKE_GUARANTOR(cond, action) \
    kbase::Guarantor(cond, __FILE__, __LINE__, kbase::EnsureAction::##action)

#define ENSURE(action, cond) \
    (!ACTION_IS_ON(action) || (cond)) ? (void)0 : MAKE_GUARANTOR(#cond, action).GUARANTOR_A

class Guarantor {
public:
    Guarantor(const char* msg, const char* file_name, int line, EnsureAction action)
        : action_required_(action)
    {
        // TODO: capture stack trace in constructor, but append data in final action.
        exception_desc_ << StringPrintf("Failed: %s\nFile: %s Line: %d\nCurrent Variables:\n",
                                        msg, file_name, line);
    }

    ~Guarantor() = default;

    DISALLOW_COPY(Guarantor);

    DISALLOW_MOVE(Guarantor);

    // Capture diagnostic variables.

    template<typename T>
    Guarantor& CaptureValue(const char* name, T&& value)
    {
        exception_desc_ << "    " << name << " = " << value << "\n";
        return *this;
    }

    Guarantor& CaptureValue(const char* name, const std::wstring& value)
    {
        std::string converted = SysWideToUTF8(value);
        return CaptureValue(name, converted);
    }

    Guarantor& CaptureValue(const char* name, const wchar_t* value)
    {
        std::string converted = SysWideToUTF8(value);
        return CaptureValue(name, converted);
    }

    void Require() const;

    void Require(const std::string msg);

    // Access stubs for infinite variable capture.
    Guarantor& GUARANTOR_A = *this;
    Guarantor& GUARANTOR_B = *this;

private:
    void Check() const;

    void Raise() const;

    void RaiseWithDump() const;

private:
    EnsureAction action_required_;
    std::ostringstream exception_desc_;
};

// This class automatically retrieves the last error code of the calling thread when
// constructing an instance, and stores the value internally.
class LastError {
public:
    LastError();

    ~LastError() = default;

    unsigned long error_code() const;

    // Since the description of the error is intended for programmers only, the
    // function insists on using English as its dispalying language.
    std::wstring GetDescriptiveMessage() const;

private:
    unsigned long error_code_;
};

void EnableAlwaysCheckForEnsureInDebug(bool always_check);

class Win32Exception : public std::runtime_error {
public:
    Win32Exception(unsigned long last_error, const std::string& message);

    unsigned long error_code() const;

private:
    unsigned long error_code_;
};

#define ThrowLastErrorIf(exp, msg)                                                \
    kbase::ThrowLastErrorIfInternal(__FILE__, __LINE__, __FUNCTION__, exp, msg)

// Throws a Win32Exception if |expression| is true.
// This function internally displays description of the last error, which means that
// ex.what() does return text like "user_message (descption_of_last_error)"
void ThrowLastErrorIfInternal(const char* file, int line, const char* fn_name,
                              bool expression, const std::string& user_message);

}   // namespace kbase

#endif  // KBASE_ERROR_EXCEPTION_UTIL_H_