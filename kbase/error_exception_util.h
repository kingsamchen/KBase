/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_ERROR_EXCEPTION_UTIL_H_
#define KBASE_ERROR_EXCEPTION_UTIL_H_

#include <sstream>
#include <string>
#include <stdexcept>

#include "kbase/basic_macros.h"
#include "kbase/path.h"
#include "kbase/sys_string_encoding_conversions.h"

namespace kbase {

enum class EnsureAction : int {
    CHECK,
    RAISE,
    RAISE_WITH_DUMP
};

constexpr bool NotReached()
{
    return false;
}

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
    static_assert(std::is_same<std::remove_const_t<decltype(cond)>, bool>::value, \
                  "cond must be a bool expression"); \
    (!ACTION_IS_ON(action) || (cond)) ? (void)0 : MAKE_GUARANTOR(#cond, action).GUARANTOR_A

class Guarantor {
public:
    Guarantor(const char* msg, const char* file_name, int line, EnsureAction action)
        : action_required_(action)
    {
        // Keep execution in construction short, and try not to call WinAPI here,
        // which might overwrite last-error code we need, even when they succeed.
        exception_desc_ << "Failed: " << msg
                        << "\nFile: " << file_name << " Line: " << line
                        << "\nCaptured Variables:\n";
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

    void Require();

    void Require(const std::string msg);

    // Access stubs for infinite variable capture.
    Guarantor& GUARANTOR_A = *this;
    Guarantor& GUARANTOR_B = *this;

private:
    void Check();

    void Raise();

    void RaiseWithDump();

private:
    EnsureAction action_required_;
    std::ostringstream exception_desc_;
};

void EnableAlwaysCheckForEnsureInDebug(bool always_check);

// If `dump_dir` wasn't specified, current directory is used as storage location.
void SetMiniDumpDirectory(const Path& dump_dir);

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

std::ostream& operator<<(std::ostream& os, const LastError& last_error);

class ExceptionWithMiniDump : public std::runtime_error {
public:
    explicit ExceptionWithMiniDump(const Path& dump_path, const std::string& message)
        : runtime_error(message), dump_path_(dump_path)
    {}

    explicit ExceptionWithMiniDump(const Path& dump_path, const char* message)
        : runtime_error(message), dump_path_(dump_path)
    {}

    ~ExceptionWithMiniDump() = default;

    Path GetMiniDumpPath() const
    {
        return dump_path_;
    }

private:
    Path dump_path_;
};

}   // namespace kbase

#endif  // KBASE_ERROR_EXCEPTION_UTIL_H_