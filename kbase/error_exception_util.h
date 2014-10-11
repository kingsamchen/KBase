/*
 @ Kingsley Chen
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

#include "kbase/strings/string_format.h"

namespace kbase {

namespace internal {

// Defines the macro _ENSURE_DISABLED ahead of including this file to disable error
// context catching functionality.
// Be wary of that this marco does not disable assert in DEBUG mode.
#if defined(_ENSURE_DISABLED)
#define ENSURE_MODE 0
#else
#define ENSURE_MODE 1
#endif

enum { ENSURE_ON = ENSURE_MODE };

#undef ENSURE_MODE

}   // namespace internal

#define GUARANTOR_A(x) GUARANTOR_OP(x, B)
#define GUARANTOR_B(x) GUARANTOR_OP(x, A)
#define GUARANTOR_OP(x, next) \
    GUARANTOR_A.current_value(#x, (x)).GUARANTOR_ ## next

#define MAKE_GUARANTOR(exp) kbase::Guarantor(exp, __FILE__, __LINE__)

#define DO_ENSURE(exp)                                                    \
    if ((exp || !kbase::internal::ENSURE_ON)) ;                           \
    else                                                                  \
        MAKE_GUARANTOR(#exp).GUARANTOR_A     

#ifdef NDEBUG
#define ENSURE(exp) DO_ENSURE(exp)
#else
#define ENSURE(exp)                                                       \
  assert(exp);                                                            \
  DO_ENSURE(exp)
#endif

class Guarantor {
public:
    Guarantor(const char* msg, const char* file_name, int line)
    {
        std::string context  
            = StringPrintf("Failed: %s\nFile: %s Line: %d\nCurrent Variables:\n",
                           msg, file_name, line);
        exception_desc_ << context;
    }

    ~Guarantor() = default;

    Guarantor(const Guarantor&) = delete;

    Guarantor& operator=(const Guarantor&) = delete;

    // Incorporates variable value.
    template<typename T>
    Guarantor& current_value(const char* name, const T& value)
    {
        exception_desc_ << "    " << name << " = " << value << "\n";
        return *this;
    }

    void raise()
    {
        throw std::runtime_error(exception_desc_.str());
    }

    // access stubs
    Guarantor& GUARANTOR_A = *this;
    Guarantor& GUARANTOR_B = *this;

private:
    std::ostringstream exception_desc_;
};

// This class automatically retrieves the last error code of the calling thread when
// constructing an instance, and stores the value internally.
class LastError {
public:
    LastError();

    unsigned long last_error_code() const;

    // Since the description of the error is intended for programmers only, the
    // function insists on using English as its dispalying language.
    std::wstring GetVerboseMessage() const;

private:
    unsigned long error_code_;
};

class Win32Exception : public std::runtime_error {
public:
    Win32Exception(unsigned long last_error, const std::string& message);
    
    unsigned long error_code() const;

private:
    unsigned long error_code_;
};

// Throws a Win32Exception if |expression| is true.
// This function internally displays description of the last error, which means that
// ex.what() does return text like "user_message (descption_of_last_error)"
void ThrowLastErrorIf(bool expression, const std::string& user_message);

}   // namespace kbase

#endif  // KBASE_ERROR_EXCEPTION_UTIL_H_