/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_BASE64_H_
#define KBASE_BASE64_H_

#include <vector>

#include "kbase\basic_types.h"

namespace kbase {

std::vector<byte> Base64Encode(const void* data, size_t len);

std::vector<byte> Base64Decode(const void* data, size_t len);

std::string Base64Encode(const std::string& src);

std::string Base64Decode(const std::string& src);

}   // namespace kbase

#endif  // KBASE_BASE64_H_