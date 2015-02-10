
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SHA_H_
#define KBASE_SHA_H_

#include <array>

namespace kbase {

using SHA1Digest = std::array<unsigned char, 20>;
using SHA256Digest = std::array<unsigned char, 32>;

void SHA1Sum(const void* data, size_t size, SHA1Digest* digest);

std::string SHA1String(const std::string& str);

void SHA256Sum(const void* data, size_t size, SHA256Digest* digest);

std::string SHA256String(const std::string& str);

std::string SHADigestToString(const SHA1Digest& digest);

std::string SHADigestToString(const SHA256Digest& digest);

}   // namespace

#endif