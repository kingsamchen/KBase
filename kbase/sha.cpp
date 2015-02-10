
#include "kbase\sha.h"

#include <Windows.h>

#include "kbase\error_exception_util.h"
#include "kbase\scope_guard.h"

namespace {

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
    BOOL rv = CryptAcquireContextW(&provider, nullptr, nullptr, PROV_RSA_FULL,
                                   CRYPT_VERIFYCONTEXT);
    ThrowLastErrorIf(!rv, "CryptAcquireContext failed.");

    {
        // Acquire hash object.
        HCRYPTHASH hash = 0;
        ON_SCOPE_EXIT([&hash] { if (hash) CryptDestroyHash(hash); });
        rv = CryptCreateHash(provider, type, 0, 0, &hash);
        ThrowLastErrorIf(!rv, "CryptCreateHash failed.");

        // Hash data.
        rv = CryptHashData(hash,
                           static_cast<const BYTE*>(data),
                           static_cast<DWORD>(size),
                           0);
        ThrowLastErrorIf(!rv, "CryptHashData failed.");

        // Retrieve it back.
        DWORD hash_len = 0;
        DWORD buffer_size = sizeof(hash_len);
        rv = CryptGetHashParam(hash, HP_HASHSIZE, reinterpret_cast<BYTE*>(&hash_len),
                               &buffer_size, 0);
        ThrowLastErrorIf(!rv, "CryptGetHashParam for HASHSIZE failed.");
        ThrowLastErrorIf(hash_len != digest->size(), "Hash value is wrong length.");

        rv = CryptGetHashParam(hash,
                               HP_HASHVAL,
                               reinterpret_cast<BYTE*>(digest->data()),
                               &hash_len,
                               0);
        ThrowLastErrorIf(!rv, "CryptGetHashParam for HASHVAL failed.");
    }
}

}   // namespace

namespace kbase {

void SHA1Sum(const void* data, size_t size, SHA1Digest* digest)
{
    SHAHash(data, size, SHAType::SHA_1, digest);
}

}   // namespace kbase