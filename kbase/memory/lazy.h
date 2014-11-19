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
    typedef std::function<T*()> Constructor;

    Lazy();

    explicit Lazy(Constructor ctor);

    Lazy(const Lazy&) = delete;

    Lazy(Lazy&&) = delete;

    Lazy& operator=(const Lazy&) = delete;

    Lazy& operator=(Lazy&&) = delete;

    ~Lazy() = default;

    T& value();

private:
    std::unique_ptr<T> value_;
    Constructor ctor_;
    std::once_flag flag_;
};

}   // namespace kbase

#endif  // KBASE_MEMORY_LAZY_H_