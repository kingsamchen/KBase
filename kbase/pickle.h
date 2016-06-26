/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_PICKLE_H_
#define KBASE_PICKLE_H_

#include <cstdint>

#include "kbase/basic_types.h"
#include "kbase/error_exception_util.h"

namespace kbase {

class Pickle;

class PickleReader {
public:
    explicit PickleReader(const Pickle& pickle);

    bool Read(bool* result);

    bool Read(int* result);

    bool Read(uint32_t* result);

    bool Read(int64_t* result);

    bool Read(uint64_t* result);

    bool Read(float* result);

    bool Read(double* result);

    bool Read(std::string* result);

    bool Read(std::wstring* result);

    bool ReadBytes(const char** data, int length);

    bool ReadData(const char** data, int* read_length);

    bool SkipBytes(int num_bytes);

private:
    template<typename T>
    inline bool ReadBuiltIninType(T* result);

    template<typename T>
    inline const char* GetReadPointerAndAdvance();

    const char* GetReadPointerAndAdvance(int num_bytes);

    // When the size of element doesn't equal to sizeof(char), use this function
    // for safety consieration. this function runs overflow check on int32 num_bytes.
    const char* GetReadPointerAndAdvance(int num_elements, size_t element_size);

private:
    const char* read_ptr_;
    const char* read_end_ptr_;
};


// Underlying memory layout:
// <---------------- capacity -------------->
// +------+-----+-----+-+-----+---+-----+---+
// |header|seg_1|seg_2|#|seg_3|...|seg_n|   |
// +------+-----+-----+-+-----+---+-----+---+
//        <---------- payload ---------->
// Note that, every segment starts on the address that is 4-byte aligned, thus
// there might be a padding between two logically consecutive segments.

class Pickle {
private:
    struct Header {
        uint32_t payload_size;
    };

public:
    Pickle();

    // Creates from a given serialized buffer.
    Pickle(const void* data, size_t size_in_bytes);

    Pickle(const Pickle& other);

    Pickle(Pickle&& other) noexcept;

    Pickle& operator=(const Pickle& rhs);

    Pickle& operator=(Pickle&& rhs) noexcept;

    ~Pickle();

    const void* data() const noexcept
    {
        return header_;
    }

    // Returns the size of internal data, including header, in bytes.
    size_t size() const noexcept
    {
        ENSURE(CHECK, header_ != nullptr).Require();
        return sizeof(Header) + header_->payload_size;
    }

    const void* payload() const noexcept
    {
        return header_ + 1;
    }

    size_t payload_size() const noexcept
    {
        ENSURE(CHECK, header_ != nullptr).Require();
        return header_->payload_size;
    }

    // Returns true, if no payload.
    // Returns false, otherwise.
    bool payload_empty() const noexcept
    {
        return payload_size() == 0;
    }

    Pickle& operator<<(short value)
    {
        Write(&value, sizeof(value));
        return *this;
    }

    Pickle& operator<<(unsigned short value)
    {
        Write(&value, sizeof(value));
        return *this;
    }

    Pickle& operator<<(bool value)
    {
        Write(&value, sizeof(value));
        return *this;
    }

    Pickle& operator<<(int value)
    {
        Write(&value, sizeof(value));
        return *this;
    }

    Pickle& operator<<(unsigned int value)
    {
        Write(&value, sizeof(value));
        return *this;
    }

    Pickle& operator<<(int64_t value)
    {
        Write(&value, sizeof(value));
        return *this;
    }

    Pickle& operator<<(uint64_t value)
    {
        Write(&value, sizeof(value));
        return *this;
    }

    Pickle& operator<<(float value)
    {
        Write(&value, sizeof(value));
        return *this;
    }

    Pickle& operator<<(double value)
    {
        Write(&value, sizeof(value));
        return *this;
    }

    Pickle& operator<<(const std::string& value);

    Pickle& operator<<(const std::wstring& value);

    // Serializes data in byte with specified length.
    void Write(const void* data, size_t size_in_bytes);

private:
    // Resizes the capacity of the internal buffer. This function internally rounds the
    // `new_capacity` up to the nearest multiple of predefined storage unit.
    void ResizeCapacity(size_t new_capacity);

    // Locates to an uint32-aligned offset as the starting position, and resizes
    // the internal buffer if free space is less than demand(padding plus `length`).
    byte* SeekWritePosition(size_t length);

    byte* mutable_payload() const noexcept
    {
        return const_cast<byte*>(static_cast<const byte*>(payload()));
    }

    const void* end_of_payload() const noexcept
    {
        return static_cast<const byte*>(payload()) + payload_size();
    }

private:
    static constexpr const size_t kCapacityUnit = 64U;
    Header* header_;
    size_t capacity_;

    friend class PickleReader;
};

}   // namespace kbase

#endif  // KBASE_PICKLE_H_