/*
 @ Kingsley Chen
*/

#include "kbase\base64.h"

namespace {

using kbase::byte;

const char kPadding = '=';

const char kCipher0[256] {
    'A', 'A', 'A', 'A', 'B', 'B', 'B', 'B', 'C', 'C', 'C', 'C',
    'D', 'D', 'D', 'D', 'E', 'E', 'E', 'E', 'F', 'F', 'F', 'F',
    'G', 'G', 'G', 'G', 'H', 'H', 'H', 'H', 'I', 'I', 'I', 'I',
    'J', 'J', 'J', 'J', 'K', 'K', 'K', 'K', 'L', 'L', 'L', 'L',
    'M', 'M', 'M', 'M', 'N', 'N', 'N', 'N', 'O', 'O', 'O', 'O',
    'P', 'P', 'P', 'P', 'Q', 'Q', 'Q', 'Q', 'R', 'R', 'R', 'R',
    'S', 'S', 'S', 'S', 'T', 'T', 'T', 'T', 'U', 'U', 'U', 'U',
    'V', 'V', 'V', 'V', 'W', 'W', 'W', 'W', 'X', 'X', 'X', 'X',
    'Y', 'Y', 'Y', 'Y', 'Z', 'Z', 'Z', 'Z', 'a', 'a', 'a', 'a',
    'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'd', 'd', 'd', 'd',
    'e', 'e', 'e', 'e', 'f', 'f', 'f', 'f', 'g', 'g', 'g', 'g',
    'h', 'h', 'h', 'h', 'i', 'i', 'i', 'i', 'j', 'j', 'j', 'j',
    'k', 'k', 'k', 'k', 'l', 'l', 'l', 'l', 'm', 'm', 'm', 'm',
    'n', 'n', 'n', 'n', 'o', 'o', 'o', 'o', 'p', 'p', 'p', 'p',
    'q', 'q', 'q', 'q', 'r', 'r', 'r', 'r', 's', 's', 's', 's',
    't', 't', 't', 't', 'u', 'u', 'u', 'u', 'v', 'v', 'v', 'v',
    'w', 'w', 'w', 'w', 'x', 'x', 'x', 'x', 'y', 'y', 'y', 'y',
    'z', 'z', 'z', 'z', '0', '0', '0', '0', '1', '1', '1', '1',
    '2', '2', '2', '2', '3', '3', '3', '3', '4', '4', '4', '4',
    '5', '5', '5', '5', '6', '6', '6', '6', '7', '7', '7', '7',
    '8', '8', '8', '8', '9', '9', '9', '9', '+', '+', '+', '+',
    '/', '/', '/', '/'
};

const char kCipher1[64] {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/'
};

const char kCipher2[256] {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
    's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C', 'D',
    'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
    'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/'
};

// ceil(PTL / 3) * 4 == floor(PTL + 3 - 1 / 3) * 4
inline size_t EncodeLength(size_t plain_text_len)
{
    return (plain_text_len + 2) / 3 * 4;
}

template<typename Container>
void Encode(const byte* data, size_t len, Container* result)
{
    result->clear();
    result->resize(EncodeLength(len), 0);

    byte t0, t1, t2;
    size_t i = 0;
    char* p = &(*result)[0];

    if (len > 2) {
        for (; i < len - 2; i += 3) {
            t0 = data[i];
            t1 = data[i+1];
            t2 = data[i+2];
            *p++ = kCipher0[t0];
            *p++ = kCipher1[((t0 & 0x03) << 4) | ((t1 >> 4) & 0x0F)];
            *p++ = kCipher1[((t1 & 0x0F) << 2) | ((t2 >> 6) & 0x03)];
            *p++ = kCipher2[t2];
        }
    }

    switch (len - i) {
    case 0:
        break;
    case 1:
        t0 = data[i];
        *p++ = kCipher0[t0];
        *p++ = kCipher1[(t0 & 0x03) << 4];
        *p++ = kPadding;
        *p = kPadding;
        break;
    case 2:
        t0 = data[i];
        t1 = data[i+1];
        *p++ = kCipher0[t0];
        *p++ = kCipher1[((t0 & 0x03) << 4) | ((t1 >> 4) & 0x0F)];
        *p++ = kCipher1[(t1 & 0x0F) << 2];
        *p = kPadding;
        break;
    }
}

template<typename Container>
void Decode(const byte* data, size_t len, Container* result)
{

}

}   // namespace

namespace kbase {

std::string Base64Encode(const std::string& src)
{
    std::string encoded;
    Encode<std::string>(reinterpret_cast<const byte*>(src.data()), src.size(), &encoded);

    return encoded;
}

std::string Base64Encode(const void* data, size_t len)
{
    std::string encoded;
    Encode<std::string>(static_cast<const byte*>(data), len, &encoded);

    return encoded;
}

}   // namespace kbase