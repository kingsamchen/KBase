/*
 @ Kingsley Chen
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_AUTO_RESET_H_
#define KBASE_AUTO_RESET_H_

namespace KBase {

// be cautious of that an AutoReset object must have shorter lifetime than
// the scoped_var object. otherwise, it may cause invalid memory reference
// during destruction.
template<typename T>
class AutoReset {
public:
    AutoReset(T* scoped_var, T new_value)
        : scoped_var_(scoped_var), original_value(*scoped_var)
    {
        *scoped_var_ = new_value;
    }

    ~AutoReset()
    {
        *scoped_var_ = original_value;
    }

    AutoReset(const AutoReset&) = delete;
    AutoReset& operator=(const AutoReset&) = delete;

private:
    T* scoped_var_;
    T original_value;
};

}   // namespace KBase

#endif  // KBASE_AUTO_RESET_H_