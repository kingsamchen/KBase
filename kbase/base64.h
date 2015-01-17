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

// Encode a string or a chunk of data into base64.
// Padding is mandatory.

std::string Base64Encode(const std::string& src);

std::string Base64Encode(const void* data, size_t len);

// Decode a string or a chunk of data that has been encoded in base64.
// Returns an empty string or vector if the input is not a valid base64 string.

std::string Base64Decode(const std::string& src);

std::vector<byte> Base64Decode(const void* data, size_t len);

}   // namespace kbase

#endif  // KBASE_BASE64_H_