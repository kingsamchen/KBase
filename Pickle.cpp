
#include "Pickle.h"

#include <cassert>
#include <cstdlib>

namespace KBase{

// static
const int Pickle::kPayloadUnit = 64;
static const int kCapacityReadOnly = static_cast<size_t>(-1);

Pickle::Pickle() : header_(nullptr), capacity_(0), buffer_offset(0)
{
    Resize(kPayloadUnit);
    header_->payload_size = 0;
}


Pickle::~Pickle()
{
}

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
