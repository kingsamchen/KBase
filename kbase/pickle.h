/*
 @ Kingsley Chen
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_PICKLE_H_
#define KBASE_PICKLE_H_

#include <cstdint>
#include <string>

namespace kbase {

class Pickle;

class PickleIterator {
public:
    PickleIterator() : read_ptr_(nullptr), read_end_ptr_(nullptr) {}

    explicit PickleIterator(const Pickle& pickle);

    bool ReadBool(bool* result);

    bool ReadInt(int* result);

    bool ReadUInt32(uint32_t* result);

    bool ReadInt64(int64_t* result);

    bool ReadUInt64(uint64_t* result);
    
    bool ReadFloat(float* result);
    
    bool ReadDouble(double* result);
    
    bool ReadString(std::string* result);
    
    bool ReadWString(std::wstring* result);
    
    bool ReadBytes(const char** data, int length);
    
    bool ReadData(const char** data, int* read_length);
    
    bool SkipBytes(int num_bytes);

private:
    template<typename T>
    inline bool ReadBuiltIninType(T* result);
    
    template<typename T>
    inline const char* GetReadPointerAndAdvance();
    
    const char* GetReadPointerAndAdvance(int num_bytes);

    /*
     @ brief
        when the size of element doesn't equal to sizeof(char), use this function
        for safety consieration. this function runs overflow check on int32 num_bytes.
    */
    const char* GetReadPointerAndAdvance(int num_elements, size_t element_size);

private:
    const char* read_ptr_;
    const char* read_end_ptr_;
};


// Note: every segment in payload is uint32 aligned, thus there might be a gap
// between two logically consective segments.
// <------------capacity----------->
// +------+----+----+---+----+-----+
// |header|seg1|seg2|...|segn|     |
// +------+----+----+---+----+-----+
//        <-----payload------>

class Pickle {
public:
    Pickle();

    Pickle(const char* data, int data_len);

    /*
     @ brief
        a deep copy of an Pickle object
    */
    Pickle(const Pickle& other);
    
    ~Pickle();

    Pickle& operator=(const Pickle& rhs);

    inline size_t size() const;
    
    inline const void* data() const;
    
    inline bool WriteBool(bool value);
    
    inline bool WriteInt(int value);
    
    inline bool WriteUInt32(uint32_t value);
    
    inline bool WriteInt64(int64_t value);
    
    inline bool WriteUInt64(uint64_t value);
    
    inline bool WriteFloat(float value);
    
    inline bool WriteDouble(double value);
    
    bool WriteString(const std::string& value);
    
    bool WriteWString(const std::wstring& value);

    /*
     @ brief
        serialize data in byte with specified length. PoD types only
        the function guarantees the internal data remains unchanged if this
        funtion fails.
    */
    bool WriteByte(const void* data, int data_len);

    bool WriteData(const char* data, int length);

    struct Header {
        uint32_t payload_size;
    };

    inline char* mutable_payload() const;
    
    inline const char* payload() const;
    
    inline const char* end_of_payload() const;
    
    inline size_t payload_size() const;

private:
    /*
     @ brief
        resize the capacity of internal buffer. this function internally rounds the
        [new_capacity] up to the next multiple of predefined alignment
     @ params
        new_capacity[in] new capacity of internal buffer and will be aligned internally.
        be wary of that, the new_capacity actually includes internal header size.
        e.g. new_capacity = header_size + your_desired_payload_size
    */
    bool Resize(size_t new_capacity);
    
    static size_t AlignInt(size_t i, int alignment);
    
    /*
     @ brief
        locate to the next uint32-aligned offset. and resize internal buffer if
        necessary.
     @ return
        the location that the data should be written at, or
        nullptr if an error occured.
    */
    char* BeginWrite(size_t length);
    
    /*
     @ brief
        zero pading memory; otherwise some memory detectors may complain about
        uninitialized memory.
    */
    void EndWrite(char* dest, size_t length);

private:
    static const int kPayloadUnit;
    
    Header* header_;
    
    size_t capacity_;
    
    size_t buffer_offset_;

    friend class PickleIterator;
/////////// exposed as dump function /////////
#ifdef _DEBUG
    friend void _pickle_dump(const Pickle& pk);
#endif
};

inline size_t Pickle::size() const
{
    return sizeof(Header) + header_->payload_size;
}

inline const void* Pickle::data() const
{
    return header_;
}

inline size_t Pickle::payload_size() const
{
    return header_->payload_size;
}

inline char* Pickle::mutable_payload() const
{
    return const_cast<char*>(payload());
}

inline const char* Pickle::payload() const
{
    return reinterpret_cast<const char*>(header_) + sizeof(Header);
}

inline const char* Pickle::end_of_payload() const
{
    return payload() + payload_size();
}

inline bool Pickle::WriteBool(bool value)
{
    return WriteInt(value ? 1 : 0);
}

inline bool Pickle::WriteInt(int value)
{
    return WriteByte(&value, sizeof(value));
}

inline bool Pickle::WriteUInt32(uint32_t value)
{
    return WriteByte(&value, sizeof(value));
}
inline bool Pickle::WriteInt64(int64_t value)
{
    return WriteByte(&value, sizeof(value));
}
inline bool Pickle::WriteUInt64(uint64_t value)
{
    return WriteByte(&value, sizeof(value));
}
inline bool Pickle::WriteFloat(float value)
{
    return WriteByte(&value, sizeof(value));
}
inline bool Pickle::WriteDouble(double value)
{
    return WriteByte(&value, sizeof(value));
}

}   // namespace kbase

#endif  // KBASE_PICKLE_H_
