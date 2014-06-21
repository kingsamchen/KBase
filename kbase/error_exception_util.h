/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_ERROR_EXCEPTION_UTIL_H_
#define KBASE_ERROR_EXCEPTION_UTIL_H_

#include <string>
#include <stdexcept>

namespace kbase {

class LastError {
public:
    LastError();

    unsigned long last_error_code() const;
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