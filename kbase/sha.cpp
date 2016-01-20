/*
 @ 0xCCCCCCCC
*/

#include "kbase/sha.h"

#include <Windows.h>

#include "kbase/error_exception_util.h"
#include "kbase/scope_guard.h"

namespace {

using kbase::LastError;

enum SHAType : unsigned int {
    SHA_1 = CALG_SHA1,
    SHA_256 = CALG_SHA_256
};

template<typename Digest>
void SHAHash(const void* data, size_t size, SHAType type, Digest* digest)
{
    // Acquire service provider.
    HCRYPTPROV provider = 0;
    ON_SCOPE_EXIT([&provider] { if (provider) CryptReleaseContext(provider, 0); });
    BOOL rv = CryptAcquireContextW(&provider, nullptr, nullptr, PROV_RSA_AES,
                                   CRYPT_VERIFYCONTEXT);
    ENSURE(RAISE, rv != 0)(LastError()).Require("CryptAcquireContext failed.");

    {
        // Acquire hash object.
        HCRYPTHASH hash = 0;
        ON_SCOPE_EXIT([&hash] { if (hash) CryptDestroyHash(hash); });
        rv = CryptCreateHash(provider, type, 0, 0, &hash);
        ENSURE(RAISE, rv != 0)(LastError()).Require("CryptCreateHash failed.");

        // Hash data.
        rv = CryptHashData(hash,
                           static_cast<const BYTE*>(data),
                           static_cast<DWORD>(size),
                           0);
        ENSURE(RAISE, rv != 0)(LastError()).Require("CryptHashData failed.");

        // Retrieve it back.
        DWORD hash_len = 0;
        DWORD buffer_size = sizeof(hash_len);
        rv = CryptGetHashParam(hash, HP_HASHSIZE, reinterpret_cast<BYTE*>(&hash_len),
                               &buffer_size, 0);
        ENSURE(RAISE, rv != 0)(LastError()).Require("CryptGetHashParam for HASHSIZE failed.");
        ENSURE(CHECK, hash_len == digest->size())(LastError())(hash_len)(digest->size());

        rv = CryptGetHashParam(hash,
                               HP_HASHVAL,
                               reinterpret_cast<BYTE*>(digest->data()),
                               &hash_len,
                               0);
        ENSURE(RAISE, rv != 0)(LastError()).Require("CryptGetHashParam for HASHVAL failed.");
    }
}

template<typename Digest>
std::string DigestToString(const Digest& digest)
{
    const char kHexDigits[] = "0123456789abcdef";

    std::string str;
    str.reserve(digest.size());
    for (unsigned char n : digest) {
        str += kHexDigits[(n >> 4) & 0x0F];
        str += kHexDigits[n & 0x0F];
    }

    return str;
}

}   // namespace

namespace kbase {

void SHA1Sum(const void* data, size_t size, SHA1Digest* digest)
{
    SHAHash(data, size, SHAType::SHA_1, digest);
}

std::string SHA1String(const std::string& str)
{
    SHA1Digest digest;
    SHA1Sum(str.data(), str.size(), &digest);

    return SHADigestToString(digest);
}

void SHA256Sum(const void* data, size_t size, SHA256Digest* digest)
{
    SHAHash(data, size, SHAType::SHA_256, digest);
}

std::string SHA256String(const std::string& str)
{
    SHA256Digest digest;
    SHA256Sum(str.data(), str.size(), &digest);

    return SHADigestToString(digest);
}

std::string SHADigestToString(const SHA1Digest& digest)
{
    return DigestToString(digest);
}

std::string SHADigestToString(const SHA256Digest& digest)
{
    return DigestToString(digest);
}

}   // namespace kbase