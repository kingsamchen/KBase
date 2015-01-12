/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_AUTO_RESET_H_
#define KBASE_AUTO_RESET_H_

namespace kbase {

// Be cautious of that an AutoReset object must have shorter lifetime than
// the scoped_var object. Otherwise, it may cause invalid memory reference
// during destruction.
template<typename T>
class AutoReset {
public:
    AutoReset(T* scoped_var)
        : scoped_var_(scoped_var), original_value_(*scoped_var)
    {}

    ~AutoReset()
    {
        *scoped_var_ = std::move(original_value_);
    }

    AutoReset(const AutoReset&) = delete;

    AutoReset& operator=(const AutoReset&) = delete;

private:
    T* scoped_var_;
    T original_value_;
};

}   // namespace kbase

#endif  // KBASE_AUTO_RESET_H_