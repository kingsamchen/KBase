/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SCOPED_HANDLE_H_
#define KBASE_SCOPED_HANDLE_H_

#include <Windows.h>

#include <cstdio>
#include <cstddef>

#include "kbase\basic_macros.h"

namespace kbase {

template<typename HandleType>
struct HandleTraits {
    using Handle = HandleType;

    HandleTraits() = delete;
    ~HandleTraits() = delete;
};

template<>
struct HandleTraits<HANDLE> {
    using Handle = HANDLE;

    HandleTraits() = delete;

    ~HandleTraits() = delete;

    static Handle NullHandle()
    {
        return nullptr;
    }

    static bool IsValid(Handle handle)
    {
        return handle != nullptr && handle != INVALID_HANDLE_VALUE;
    }

    static void Close(Handle handle)
    {
        CloseHandle(handle);
    }
};

template<>
struct HandleTraits<FILE*> {
    using Handle = FILE*;

    HandleTraits() = delete;

    ~HandleTraits() = delete;

    static Handle NullHandle()
    {
        return nullptr;
    }

    static bool IsValid(Handle handle)
    {
        return handle != nullptr;
    }

    static void Close(Handle handle)
    {
        fclose(handle);
    }
};

template<typename HandleType, typename TraitsType = HandleTraits<HandleType>>
class ScopedHandle {
public:
    using Handle = HandleType;
    using Traits = TraitsType;

    ScopedHandle() = default;

    explicit ScopedHandle(Handle handle)
        : handle_(handle)
    {}

    ScopedHandle(ScopedHandle&& other)
    {
        *this = std::move(other);
    }

    ~ScopedHandle()
    {
        *this = nullptr;
    }

    ScopedHandle& operator=(ScopedHandle&& rhs)
    {
        if (this != &rhs) {
            Close();
            handle_ = rhs.handle_;
            rhs.handle_ = Traits::NullHandle();
        }

        return *this;
    }

    DISALLOW_COPY(ScopedHandle);

    // A convenient way for destroying the object.
    ScopedHandle& operator=(nullptr_t)
    {
        Close();
        return *this;
    }

    explicit operator bool() const
    {
        return Traits::IsValid(handle_);
    }

    // Be careful.
    operator Handle() const
    {
        return handle_;
    }

    void Close()
    {
        if (Traits::IsValid(handle_)) {
            Traits::Close(handle_);
            handle_ = Traits::NullHandle();
        }
    }

    // Releases the ownership of the managed handle.
    // Returns managed handle or NullHandle if there was no managed object.
    Handle Release()
    {
        Handle self = handle_;
        handle_ = Traits::NullHandle();

        return self;
    }

    Handle Get() const
    {
        return handle_;
    }

    // Replaces the managed handle.
    void Reset(Handle new_handle)
    {
        Close();
        handle_ = new_handle;
    }

    void swap(ScopedHandle& other)
    {
        using std::swap;
        swap(handle_, other.handle_);
    }

private:
    Handle handle_ = Traits::NullHandle();
};

// Specialized version for std::swap.
template<typename HandleType, typename TraitsType = HandleTraits<HandleType>>
void swap(ScopedHandle<HandleType>& lhs, ScopedHandle<HandleType>& rhs)
{
    lhs.swap(rhs);
}

using ScopedSysHandle = ScopedHandle<HANDLE>;
using ScopedStdioHandle = ScopedHandle<FILE*>;

}   // namespace kbase

#endif  // KBASE_SCOPED_HANDLE_H_