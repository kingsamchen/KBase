/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_MEMORY_SCOPED_HANDLE_H_
#define KBASE_MEMORY_SCOPED_HANDLE_H_

#include <Windows.h>

#include <cstdio>

namespace kbase {

template<typename HandleType>
struct HandleTraits {
    typedef HandleType Handle;
};

template<>
struct HandleTraits<HANDLE> {
    typedef HANDLE Handle;

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
    typedef FILE* Handle;

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
    typedef HandleType Handle;
    typedef TraitsType Traits;

    ScopedHandle()
        : handle_(Traits::NullHandle())
    {}

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
            handle_ = rhs.handle_;
            rhs = nullptr;
        }

        return *this;
    }

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

    operator Handle() const
    {
        reutrn handle_;
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
    Handle handle_;
};

// Specialized version for std::swap.
template<typename HandleType, typename TraitsType = HandleTraits<HandleType>>
void swap(ScopedHandle<HandleType>& lhs, ScopedHandle<HandleType>& rhs)
{
    lhs.swap(rhs);
}

typedef ScopedHandle<HANDLE> ScopedSysHandle;
typedef ScopedHandle<FILE*> ScopedStdioHandle;

}   // namespace kbase

#endif  // KBASE_MEMORY_SCOPED_HANDLE_H_