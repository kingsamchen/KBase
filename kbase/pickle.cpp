/*
 @ 0xCCCCCCCC
*/

#include "kbase/pickle.h"

#include <algorithm>

#include "kbase/error_exception_util.h"

namespace {

// Rounds up `num` to the nearest multiple of `factor`.
constexpr size_t RoundToMultiple(size_t num, size_t factor)
{
    return factor == 0 ? 0 : (num - 1 - (num - 1) % factor + factor);
}

}   // namespace

namespace kbase {

PickleReader::PickleReader(const Pickle& pickle)
    : read_ptr_(pickle.payload()), read_end_ptr_(pickle.end_of_payload())
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

// Payload is uint32 aligned.

Pickle::Pickle()
    : header_(nullptr), capacity_(0)
{
    Resize(kCapacityUnit);
    header_->payload_size = 0;
}

Pickle::Pickle(const char* data, int)
    : header_(reinterpret_cast<Header*>(const_cast<char*>(data))),
      capacity_(kCapacityReadOnly)
{}

Pickle::Pickle(const Pickle& other)
    : header_(nullptr), capacity_(0)
{
    // If |other| is constructed from a const buffer, its capacity value is
    // kCapacityReadOnly. therefore, calculate its capacity on hand.
    size_t capacity = sizeof(Header) + other.header_->payload_size;
    Resize(capacity);

    memcpy_s(header_, capacity_, other.header_, capacity);
}

Pickle::Pickle(Pickle&& other)
{
    *this = std::move(other);
}

Pickle& Pickle::operator=(const Pickle& rhs)
{
    if (this == &rhs) {
        return *this;
    }

    if (capacity_ == kCapacityReadOnly) {
        header_ = nullptr;
        capacity_ = 0;
    }

    size_t capacity = sizeof(Header) + rhs.header_->payload_size;
    Resize(capacity);

    memcpy_s(header_, capacity_, rhs.header_, capacity);

    return *this;
}

Pickle& Pickle::operator=(Pickle&& rhs)
{
    if (this != &rhs) {
        header_ = rhs.header_;
        capacity_ = rhs.capacity_;
        // Let it die peacefully.
        rhs.capacity_ = kCapacityReadOnly;
        rhs.header_ = nullptr;
    }

    return *this;
}

Pickle::~Pickle()
{
    if (capacity_ != kCapacityReadOnly) {
        free(header_);
    }
}

void Pickle::Resize(size_t new_capacity)
{
    ENSURE(CHECK, !readonly() && new_capacity > capacity_).Require();

    new_capacity = RoundToMultiple(new_capacity, kCapacityUnit);
    void* ptr = realloc(header_, new_capacity);

    ENSURE(RAISE, ptr != nullptr).Require("Failed to realloc a new memory block!");
    header_ = static_cast<Header*>(ptr);
    capacity_ = new_capacity;
}

bool Pickle::Write(const std::string& value)
{
    if (!Write(static_cast<int>(value.size()))) {
        return false;
    }

    return WriteByte(value.data(), static_cast<int>(value.size()));
}

bool Pickle::Write(const std::wstring& value)
{
    if (!Write(static_cast<int>(value.size()))) {
        return false;
    }

    return WriteByte(value.data(), static_cast<int>(value.size() * sizeof(wchar_t)));
}

bool Pickle::WriteByte(const void* data, int data_len)
{
    if (capacity_ == kCapacityReadOnly) {
        ENSURE(CHECK, NotReached()).Require();
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

bool Pickle::WriteData(const char* data, int length)
{
    return length >= 0 && Write(length) && WriteByte(data, length);
}

char* Pickle::BeginWrite(size_t length)
{
    // Write at a uint32-aligned offset from the begining of head.
    size_t offset = RoundToMultiple(header_->payload_size, sizeof(uint32_t));
    size_t required_size = offset + length;
    size_t total_required_size = required_size + sizeof(Header);

    if (total_required_size > capacity_) {
        Resize(std::max(capacity_ << 1, total_required_size));
    }

    header_->payload_size = static_cast<uint32_t>(required_size);

    return mutable_payload() + offset;
}

void Pickle::EndWrite(char* dest, size_t length)
{
    if (length % sizeof(uint32_t)) {
        memset(dest + length, 0, sizeof(uint32_t) - (length % sizeof(uint32_t)));
    }
}

}   // namespace kbase