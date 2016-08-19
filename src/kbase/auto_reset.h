/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_AUTO_RESET_H_
#define KBASE_AUTO_RESET_H_

#include "kbase/basic_macros.h"

namespace kbase {

// Be cautious of that an AutoReset object must have shorter lifetime than
// the scoped_var object. Otherwise, it may cause invalid memory reference
// during destruction.
template<typename T>
class AutoReset {
public:
    explicit AutoReset(T* scoped_var)
        : scoped_var_(scoped_var), original_value_(*scoped_var)
    {}

    ~AutoReset()
    {
        // Our backup is discardable now, move it back may more efficient.
        *scoped_var_ = std::move(original_value_);
    }

    DISALLOW_COPY(AutoReset);

    DISALLOW_MOVE(AutoReset);

private:
    T* scoped_var_;
    T original_value_;
};

}   // namespace kbase

#endif  // KBASE_AUTO_RESET_H_