/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_MD5_H_
#define KBASE_MD5_H_

namespace kbase {

/* Any 32-bit or wider unsigned integer data type will do */
typedef unsigned int MD5uint32;

typedef struct {
    MD5uint32 lo, hi;
    MD5uint32 a, b, c, d;
    unsigned char buffer[64];
    MD5uint32 block[16];
} MD5_CTX;

void MD5_Init(MD5_CTX *ctx);

void MD5_Update(MD5_CTX *ctx, void *data, unsigned long size);

void MD5_Final(unsigned char *result, MD5_CTX *ctx);

}   // namespace kbase

#endif  // KBASE_MD5_H_