
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_PICKLE_H_
#define KBASE_PICKLE_H_

#include <cstdint>

namespace KBase {

class PickleIterator {

};

class Pickle {
public:
    Pickle();
    Pickle(const char* data, int data_len);
    Pickle(const Pickle& other);
    ~Pickle();

    Pickle& operator=(const Pickle& rhs);

    inline size_t size() const;
    inline const void* data() const;
    bool WriteByte(const void* data, int data_len);

private:
    struct Header {
        uint32_t payload_size;
    };

    bool Resize(size_t new_capacity);
    static size_t AlignInt(size_t i, int alignment);
    char* BeginWrite(size_t length);
    void EndWrite(char* dest, size_t length);
    inline char* payload() const;
    
private:
    static const int kPayloadUnit;
    Header* header_;
    size_t capacity_;
    size_t buffer_offset_;
};

inline size_t Pickle::size() const
{
    return sizeof(Header) + header_->payload_size;
}

inline const void* Pickle::data() const
{
    return header_;
}

inline char* Pickle::payload() const
{
    return reinterpret_cast<char*>(header_) + sizeof(Header);
}

}

#endif  // KBASE_PICKLE_H_
