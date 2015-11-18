/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SINGLETON_H_
#define KBASE_SINGLETON_H_

#include <mutex>

#include "kbase\at_exit_manager.h"

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
        std::call_once(flag_, &Singleton::Initialize);
        return instance_;
    }

private:
    static void Initialize()
    {
        instance_ = Traits::Create();
        if (Traits::kDestroyAtExit) {
            AtExitManager::RegisterCallback([]() {
                Traits::Destroy(instance_);
            });
        }
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

#endif  // KBASE_SINGLETON_H_