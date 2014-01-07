
#ifdef _MSC_VER > 1000
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

private:
    struct Header {
        uint32_t payload_size;
    };

    bool Resize(size_t new_capacity);
    static size_t AlignInt(size_t i, int alignment);
    
private:
    static const int kPayloadUnit;
    Header* header_;
    size_t capacity_;
    size_t buffer_offset;
};

}

#endif  // KBASE_PICKLE_H_
