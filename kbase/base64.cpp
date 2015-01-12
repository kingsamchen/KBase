/*
 @ Kingsley Chen
*/

#include "kbase\base64.h"

namespace {

using kbase::byte;

template<typename Container>
Container Encode(const byte* data, size_t len)
{
    return Container();
}

template<typename Container>
Container Decode(const byte* data, size_t len)
{
    return Container();
}

}   // namespace

namespace kbase {

}   // namespace kbase