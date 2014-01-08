
#include "Pickle.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

namespace KBase{

// static
const int Pickle::kPayloadUnit = 64;
static const int kCapacityReadOnly = static_cast<size_t>(-1);

Pickle::Pickle() : header_(nullptr), capacity_(0), buffer_offset(0)
{
    Resize(kPayloadUnit);
    header_->payload_size = 0;
}

Pickle::Pickle(const char* data, int data_len) 
    : header_(reinterpret_cast<Header*>(const_cast<char*>(data))),
      capacity_(kCapacityReadOnly),
      buffer_offset(0)
{}

/*
 @ brief
    a deep copy of an Pickle object
*/
Pickle::Pickle(const Pickle& other) : header_(nullptr), capacity_(0),
                                      buffer_offset(other.buffer_offset)
{
    // if [other] is constructed from a const buffer, its capacity value is
    // kCapacityReadOnly. therefore, calculate its capacity on hand.
    size_t capacity = sizeof(Header) + other.header_->payload_size;
    bool resized = Resize(capacity);
    assert(resized);

    memcpy_s(header_, capacity_, other.header_, capacity);
}

Pickle::~Pickle()
{
    if (capacity_ != kCapacityReadOnly) {
        free(header_);
    }
}

Pickle& Pickle::operator=(const Pickle& rhs)
{
    if (this == &rhs) {
        assert(false);
        return *this;
    }

    if (capacity_ == kCapacityReadOnly) {
        header_ = nullptr;
        capacity_ = 0;
    }

    size_t capacity = sizeof(Header) + rhs.header_->payload_size;
    bool resized = Resize(capacity);
    assert(resized);

    memcpy_s(header_, capacity_, rhs.header_, capacity);
    buffer_offset = rhs.buffer_offset;

    return *this;
}

/*
 @ brief
    resize the capacity of internal buffer. this function internally rounds the
    [new_capacity] up to the next multiple of predefined alignment
 @ params
    new_capacity[in] new capacity of internal buffer and will be aligned internally.
    be wary of that, the new_capacity actually includes internal header size.
    e.g. new_capacity = header_size + your_desired_payload_size
*/
bool Pickle::Resize(size_t new_capacity)
{
    assert(capacity_ != kCapacityReadOnly);
    new_capacity = AlignInt(new_capacity, kPayloadUnit);

    void* p = realloc(header_, new_capacity);
    if (!p) {
        return false;
    }

    header_ = static_cast<Header*>(p);
    capacity_ = new_capacity;

    return true;
}

// static
size_t Pickle::AlignInt(size_t i, int alignment)
{
    return i + (alignment - i % alignment) % alignment;
}

}
