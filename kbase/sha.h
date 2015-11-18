/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SHA_H_
#define KBASE_SHA_H_

#include <array>

namespace kbase {

// The functions that perform SHA-1/256 operations rely on crypto API provided by
// Windows.

using SHA1Digest = std::array<unsigned char, 20>;
using SHA256Digest = std::array<unsigned char, 32>;

// Calculate the SHA-1/256 checksum of a given data.
// Functions throw an exception if an error occured during invocation of
// Windows APIs.

void SHA1Sum(const void* data, size_t size, SHA1Digest* digest);

void SHA256Sum(const void* data, size_t size, SHA256Digest* digest);

// Return the SHA-1/256 checksum string, in hexadecimal, of a given string.
// Functions throw an exception if an error occured during invocation of
// Windows APIs.

std::string SHA1String(const std::string& str);

std::string SHA256String(const std::string& str);

// Convert a SHA digest into a hexadecimal string.

std::string SHADigestToString(const SHA1Digest& digest);

std::string SHADigestToString(const SHA256Digest& digest);

}   // namespace

#endif