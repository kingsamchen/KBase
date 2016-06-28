/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_PICKLE_H_
#define KBASE_PICKLE_H_

#include <cstdint>

#include "kbase/basic_macros.h"
#include "kbase/basic_types.h"
#include "kbase/error_exception_util.h"

namespace kbase {

class Pickle;

class PickleReader {
public:
    PickleReader(const void* pickled_data, size_t size_in_bytes);

    explicit PickleReader(const Pickle& pickle);

    DEFAULT_COPY(PickleReader)

    DEFAULT_MOVE(PickleReader)

    ~PickleReader() = default;

    explicit operator bool() const
    {
        return read_ptr_ < data_end_;
    }

    PickleReader& operator>>(bool& value)
    {
        Read(&value, sizeof(value));
        return *this;
    }

    PickleReader& operator>>(short& value)
    {
        Read(&value, sizeof(value));
        return *this;
    }

    PickleReader& operator>>(unsigned short& value)
    {
        Read(&value, sizeof(value));
        return *this;
    }

    PickleReader& operator>>(int& value)
    {
        Read(&value, sizeof(value));
        return *this;
    }

    PickleReader& operator>>(unsigned int& value)
    {
        Read(&value, sizeof(value));
        return *this;
    }

    PickleReader& operator>>(int64_t& value)
    {
        Read(&value, sizeof(value));
        return *this;
    }

    PickleReader& operator>>(uint64_t& value)
    {
        Read(&value, sizeof(value));
        return *this;
    }

    PickleReader& operator>>(float& value)
    {
        Read(&value, sizeof(value));
        return *this;
    }

    PickleReader& operator>>(double& value)
    {
        Read(&value, sizeof(value));
        return *this;
    }

    PickleReader& operator>>(std::string& value);

    PickleReader& operator>>(std::wstring& value);

	template<typename T>
	PickleReader& operator>>(std::vector<T>& value)
	{
		PickleReader& reader = *this;
		size_t length;
		reader >> length;
		for (size_t i = 0; i < length; ++i) {
			T ele;
			reader >> ele;
			value.push_back(std::move(ele));
		}

		return reader;
	}

    // Deserializes data in the size of `size_in_bytes`.
    void Read(void* dest, size_t size_in_bytes);

    // Skips read pointer by at least `data_size` bytes.
    void SkipData(size_t data_size) noexcept;

private:
    // Seeks to the next position by advancing at least `data_szie` bytes.
    // Any interpolated paddings would be skipped.
    void SeekReadPosition(size_t data_size) noexcept;

private:
    const byte* read_ptr_;
    const byte* data_end_;
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

    const byte* payload() const noexcept
    {
        return static_cast<const byte*>(static_cast<const void*>(header_ + 1));
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

    template<typename T>
	Pickle& operator<<(const std::vector<T>& value)
	{
		Pickle& pickle = *this;
		pickle << value.size();
		for (const auto& ele : value) {
			pickle << ele;
		}

		return pickle;
	}

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
        return const_cast<byte*>(payload());
    }

    const byte* end_of_payload() const noexcept
    {
        return payload() + payload_size();
    }

private:
    static constexpr const size_t kCapacityUnit = 64U;
    Header* header_;
    size_t capacity_;

    friend class PickleReader;
};

}   // namespace kbase

#endif  // KBASE_PICKLE_H_