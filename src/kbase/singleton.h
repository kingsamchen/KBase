/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SINGLETON_H_
#define KBASE_SINGLETON_H_

#include <mutex>

#include "kbase/at_exit_manager.h"
#include "kbase/basic_macros.h"

namespace kbase {

template<typename T>
struct DefaultSingletonTraits {
    static constexpr bool kDestroyAtExit = true;

    static T* Create()
    {
        return new T();
    }

    static void Destroy(T* instance) noexcept
    {
        delete instance;
    }
};

template<typename T>
struct LeakySingletonTraits {
    static const bool kDestroyAtExit = false;

    static T* Create()
    {
        return new T();
    }
};

template<typename T, typename Traits = DefaultSingletonTraits<T>>
class Singleton {
public:
    Singleton() = delete;

    ~Singleton() = delete;

    DISALLOW_COPY(Singleton);

    DISALLOW_MOVE(Singleton);

    static T* instance()
    {
        std::call_once(flag_, &Singleton::Initialize);
        return instance_;
    }

private:
    static void Initialize()
    {
        instance_ = Traits::Create();
        RegisterForCleanup(std::integral_constant<bool, Traits::kDestroyAtExit>());
    }

    static void RegisterForCleanup(std::true_type)
    {
        AtExitManager::RegisterCallback([]() {
            Traits::Destroy(instance_);
        });
    }

    static void RegisterForCleanup(std::false_type)
    {}

private:
    static T* instance_;
    static std::once_flag flag_;
};

template<typename T, typename Traits>
T* Singleton<T, Traits>::instance_ = nullptr;

template<typename T, typename Traits>
std::once_flag Singleton<T, Traits>::flag_;

}   // namespace kbase

#endif  // KBASE_SINGLETON_H_