/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_MEMORY_SINGLETON_H_
#define KBASE_MEMORY_SINGLETON_H_

#include <mutex>

namespace kbase {

template<typename T>
struct DefaultSingletonTraits {
    static T* Create()
    {
        return new T();
    }

    static void Destroy(T* instance)
    {
        delete instance;
    }

    static const bool kDestroyAtExit = true;
};

template<typename T>
struct LeakySingletonTraits : public DefaultSingletonTraits<T> {
    static const bool kDestroyAtExit = false;
};

template<typename T, typename Traits = DefaultSingletonTraits<T>>
class Singleton {
public:
    Singleton() = delete;

    ~Singleton() = delete;
    
    Singleton(const Singleton&) = delete;
    
    Singleton& operator=(const Singleton&) = delete;

    static T* instance()
    {
        std::call_once(flag_, Singleton::Initialize);
        return instance_;
    }

private:
    static void Initialize()
    {
        instance_ = Traits::Create();
        // TODO: Register Traits::Destroy if should cleanup at exit.
    }

private:
    static T* instance_;
    static std::once_flag flag_;
};

template<typename T, typename Traits = DefaultSingletonTraits<T>>
T* Singleton<T, Traits>::instance_ = nullptr;

template<typename T, typename Traits = DefaultSingletonTraits<T>>
std::once_flag Singleton<T, Traits>::flag_;

}   // namespace kbase

#endif  // KBASE_MEMORY_SINGLETON_H_