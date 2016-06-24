/*
 @ 0xCCCCCCCC
*/

#include "kbase/pickle.h"

#include <algorithm>

namespace {

// Rounds up `num` to the nearest multiple of `factor`.
constexpr size_t RoundToMultiple(size_t num, size_t factor)
{
    return factor == 0 ? 0 : (num - 1 - (num - 1) % factor + factor);
}

// Zeros padding memory; otherwise some memory detectors may complain about
// uninitialized memory.
void SanitizePadding(byte* padding_begin, size_t padding_size)
{
    memset(padding_begin, 0, padding_size);
}

}   // namespace

namespace kbase {

PickleReader::PickleReader(const Pickle& pickle)
    : read_ptr_(static_cast<const char*>(pickle.payload())),
      read_end_ptr_(static_cast<const char*>(pickle.end_of_payload()))
{}

bool PickleReader::Read(bool* result)
{
    return ReadBuiltIninType(result);
}

bool PickleReader::Read(int* result)
{
    return ReadBuiltIninType(result);
}

bool PickleReader::Read(uint32_t* result)
{
    return ReadBuiltIninType(result);
}

bool PickleReader::Read(int64_t* result)
{
    return ReadBuiltIninType(result);
}

bool PickleReader::Read(uint64_t* result)
{
    return ReadBuiltIninType(result);
}

bool PickleReader::Read(float* result)
{
    return ReadBuiltIninType(result);
}

bool PickleReader::Read(double* result)
{
    return ReadBuiltIninType(result);
}

bool PickleReader::Read(std::string* result)
{
    int str_length;
    if (!Read(&str_length)) {
        return false;
    }

    const char* read_from = GetReadPointerAndAdvance(str_length);
    if (!read_from) {
        return false;
    }

    result->assign(read_from, str_length);
    return true;
}

bool PickleReader::Read(std::wstring* result)
{
    int str_length;
    if (!Read(&str_length)) {
        return false;
    }

    const char* read_from = GetReadPointerAndAdvance(str_length, sizeof(wchar_t));
    if (!read_from) {
        return false;
    }

    result->assign(reinterpret_cast<const wchar_t*>(read_from), str_length);
    return true;
}

bool PickleReader::ReadBytes(const char** data, int length)
{
    const char* read_from = GetReadPointerAndAdvance(length);
    if (!read_from) {
        *data = nullptr;
        return false;
    }

    *data = read_from;
    return true;
}

bool PickleReader::ReadData(const char** data, int* read_length)
{
    if (!Read(read_length)) {
        *data = nullptr;
        *read_length = 0;
        return false;
    }

    return ReadBytes(data, *read_length);
}

bool PickleReader::SkipBytes(int num_bytes)
{
    return !!GetReadPointerAndAdvance(num_bytes);
}

// sizeof comparison in if statement causes constant expression warning
// disable the warning temporarily
#pragma warning(push)
#pragma warning(disable:4127)

template<typename T>
inline bool PickleReader::ReadBuiltIninType(T* result)
{
    const char* read_from = GetReadPointerAndAdvance<T>();

    if (!read_from) {
        return false;
    }

    if (sizeof(T) > sizeof(uint32_t)) {
        memcpy_s(result, sizeof(*result), read_from, sizeof(*result));
    } else {
        *result = *reinterpret_cast<const T*>(read_from);
    }

    return true;
}

template<typename T>
inline const char* PickleReader::GetReadPointerAndAdvance()
{
    const char* curr_read_ptr = read_ptr_;

    if (read_ptr_ + sizeof(T) > read_end_ptr_) {
        return nullptr;
    }

    // skip the memory hole if size of the type is less than uint32
    if (sizeof(T) < sizeof(uint32_t)) {
        read_ptr_ += RoundToMultiple(sizeof(T), sizeof(uint32_t));
    } else {
        read_ptr_ += sizeof(T);
    }

    return curr_read_ptr;
}

#pragma warning(pop)

const char* PickleReader::GetReadPointerAndAdvance(int num_bytes)
{
    const char* curr_read_ptr = read_ptr_;

    if (num_bytes < 0 || read_ptr_ + num_bytes > read_end_ptr_) {
        return nullptr;
    }

    read_ptr_ += RoundToMultiple(num_bytes, sizeof(uint32_t));

    return curr_read_ptr;
}

const char* PickleReader::GetReadPointerAndAdvance(int num_elements,
                                                     size_t element_size)
{
    int64_t num_bytes = static_cast<int64_t>(num_elements) * element_size;
    int num_bytes32 = static_cast<int>(num_bytes);
    if (num_bytes != static_cast<int64_t>(num_bytes32)) {
        return nullptr;
    }

    return GetReadPointerAndAdvance(num_bytes32);
}

// --* Pickle *--

Pickle::Pickle()
    : header_(nullptr), capacity_(0)
{
    ResizeCapacity(kCapacityUnit);
    header_->payload_size = 0;
}

Pickle::Pickle(const void* data, size_t size_in_bytes)
    : header_(nullptr), capacity_(0)
{
    ENSURE(CHECK, data != nullptr && size_in_bytes > 0).Require();
    ResizeCapacity(size_in_bytes);
    memcpy_s(header_, capacity_, data, size_in_bytes);
}

Pickle::Pickle(const Pickle& other)
    : header_(nullptr), capacity_(0)
{
    ResizeCapacity(other.size());
    memcpy_s(header_, capacity_, other.header_, other.size());
}

Pickle::Pickle(Pickle&& other) noexcept
    : header_(other.header_), capacity_(other.capacity_)
{
    other.header_ = nullptr;
    other.capacity_ = 0;
}

Pickle::~Pickle()
{
    // Technically, only pickles having been moved have null header.
    if (header_ != nullptr) {
        free(header_);
    }
}

Pickle& Pickle::operator=(const Pickle& rhs)
{
    if (this != &rhs) {
        if (capacity_ < rhs.size()) {
            ResizeCapacity(rhs.size());
        }

        memcpy_s(header_, capacity_, rhs.header_, rhs.size());
    }

    return *this;
}

Pickle& Pickle::operator=(Pickle&& rhs) noexcept
{
    if (this != &rhs) {
        Header* old_header = header_;

        header_ = rhs.header_;
        capacity_ = rhs.capacity_;
        rhs.header_ = nullptr;
        rhs.capacity_ = 0;

        free(old_header);
    }

    return *this;
}

void Pickle::ResizeCapacity(size_t new_capacity)
{
    ENSURE(CHECK, new_capacity > capacity_).Require();

    new_capacity = RoundToMultiple(new_capacity, kCapacityUnit);
    void* ptr = realloc(header_, new_capacity);

    ENSURE(RAISE, ptr != nullptr).Require("Failed to realloc a new memory block!");
    header_ = static_cast<Header*>(ptr);
    capacity_ = new_capacity;
}

void Pickle::Write(const std::string& value)
{
    Write(static_cast<int>(value.size()));
    Write(value.data(), static_cast<int>(value.size()));
}

void Pickle::Write(const std::wstring& value)
{
    Write(static_cast<int>(value.size()));
    Write(value.data(), static_cast<int>(value.size() * sizeof(wchar_t)));
}

void Pickle::Write(const void* data, size_t size_in_bytes)
{
    size_t last_payload_size = payload_size();
    byte* dest = SeekWritePosition(size_in_bytes);
    size_t free_buf_size = capacity_ - (dest - reinterpret_cast<byte*>(header_));
    memcpy_s(dest, free_buf_size, data, size_in_bytes);
    size_t padding_size = payload_size() - last_payload_size - size_in_bytes;
    SanitizePadding(dest - padding_size, padding_size);
}

byte* Pickle::SeekWritePosition(size_t length)
{
    // Writing starts at a uint32-aligned offset.
    size_t offset = RoundToMultiple(header_->payload_size, sizeof(uint32_t));
    size_t required_size = offset + length;
    size_t required_total_size = required_size + sizeof(Header);

    if (required_total_size > capacity_) {
        ResizeCapacity(std::max(capacity_ << 1, required_total_size));
    }

    ENSURE(CHECK, required_size <= std::numeric_limits<uint32_t>::max())(required_size).Require();
    header_->payload_size = static_cast<uint32_t>(required_size);

    return mutable_payload() + offset;
}

}   // namespace kbase