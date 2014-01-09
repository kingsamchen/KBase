
#include "Pickle.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>

namespace KBase{

// static
const int Pickle::kPayloadUnit = 64;
static const size_t kCapacityReadOnly = static_cast<size_t>(-1);

PickleIterator::PickleIterator(const Pickle& pickle)
    : read_ptr_(pickle.payload()), read_end_ptr_(pickle.end_of_payload())
{}

// payload is uint32 aligned

Pickle::Pickle() : header_(nullptr), capacity_(0), buffer_offset_(0)
{
    Resize(kPayloadUnit);
    header_->payload_size = 0;
}

Pickle::Pickle(const char* data, int) 
    : header_(reinterpret_cast<Header*>(const_cast<char*>(data))),
      capacity_(kCapacityReadOnly),
      buffer_offset_(0)
{}

/*
 @ brief
    a deep copy of an Pickle object
*/
Pickle::Pickle(const Pickle& other) : header_(nullptr), capacity_(0),
                                      buffer_offset_(other.buffer_offset_)
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
    buffer_offset_ = rhs.buffer_offset_;

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

bool Pickle::WriteString(const std::string& value)
{
    if (!WriteInt(static_cast<int>(value.size()))) {
        return false;
    }

    return WriteByte(value.data(), static_cast<int>(value.size()));
}

bool Pickle::WriteWString(const std::wstring& value)
{
    if (!WriteInt(static_cast<int>(value.size()))) {
        return false;
    }

    return WriteByte(value.data(), static_cast<int>(value.size() * sizeof(wchar_t)));
}

/*
 @ brief
    serialize data in byte with specified length. PoD types only
    the function guarantees the internal data remains unchanged if this
    funtion fails.
*/
bool Pickle::WriteByte(const void* data, int data_len)
{
    if (capacity_ == kCapacityReadOnly) {
        assert(false);
        return false;
    }

    char* dest = BeginWrite(data_len);
    if (!dest) {
        return false;
    }

    size_t extra_size = capacity_ - (dest - reinterpret_cast<char*>(header_));
    memcpy_s(dest, extra_size, data, data_len);
    EndWrite(dest, data_len);

    return true;
}

/*
 @ brief
    locate to the next uint32-aligned offset. and resize internal buffer if
    necessary.
 @ return
    the location that the data should be written at, or
    nullptr if an error occured.
*/
char* Pickle::BeginWrite(size_t length)
{
    // write at a uint32-aligned offset from the begining of head
    size_t offset = AlignInt(header_->payload_size, sizeof(uint32_t));
    size_t required_size = offset + length;
    size_t total_required_size = required_size + sizeof(Header);

    if (total_required_size > capacity_ &&
        !Resize(std::max(capacity_ << 1, total_required_size))) {
        return nullptr;
    }

    header_->payload_size = static_cast<uint32_t>(required_size);

    return mutable_payload() + offset;
}

/*
 @ brief
    zero pading memory; otherwise some memory detectors may complain about
    uninitialized memory.
*/
void Pickle::EndWrite(char* dest, size_t length)
{
    if (length % sizeof(uint32_t)) {
        memset(dest + length, 0, sizeof(uint32_t) - (length % sizeof(uint32_t)));
    }
}

}
