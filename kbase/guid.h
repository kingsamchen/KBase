/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_GUID_H_
#define KBASE_GUID_H_

#include <string>

namespace kbase {

// Returns a GUID string in the form of XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX.
// Returns an empty string if failed.
std::string GenerateGUID();

// Returns true, if the argument given is a valid GUID.
// Returns false, otherwise.
bool IsGUIDValid(const std::string& guid);

}   // namespace kbase

#endif  // KBASE_GUID_H_