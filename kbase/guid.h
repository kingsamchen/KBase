/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_GUID_H_
#define KBASE_GUID_H_

#include <string>

namespace kbase {

std::string GenerateGUID();

bool IsGUIDValid(const std::string& guid);

}   // namespace kbase

#endif  // KBASE_GUID_H_