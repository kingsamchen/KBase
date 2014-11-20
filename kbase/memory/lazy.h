/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_MEMORY_LAZY_H_
#define KBASE_MEMORY_LAZY_H_

#include <functional>
#include <memory>
#include <mutex>

namespace kbase {

template<typename T>
class Lazy {
public:
    // Creator is used when you have demand on controlling the construction of the
    // value. Each valid Creator should return a raw pointer to a T object.
    // Lazy stashes an instance of Creator during construction, and invokes it when
    // the lazy value is accessed in the first time, and then takes over the
    // ownership of the raw pointer which is returned from the Creator.
    typedef std::function<T*()> Creator;

    // Initializes the value with its default constructor.
    Lazy()
        : ctor_([]() { return new T(); })
    {}

    // Initializes the value by calling crator.
    explicit Lazy(Creator creator)
        : ctor_(creator)
    {}

    Lazy(const Lazy&) = delete;

    Lazy(Lazy&&) = delete;

    Lazy& operator=(const Lazy&) = delete;

    Lazy& operator=(Lazy&&) = delete;

    ~Lazy() = default;

    T& value()
    {
        std::call_once(flag_, &Lazy::Initialize, this);
        return *value_.get();
    }

private:
    void Initialize()
    {
        value_.reset(ctor_());
    }

private:
    std::unique_ptr<T> value_;
    Creator ctor_;
    std::once_flag flag_;
};

}   // namespace kbase

#endif  // KBASE_MEMORY_LAZY_H_