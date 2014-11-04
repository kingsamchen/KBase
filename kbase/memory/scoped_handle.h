
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

/*
    - Dev Thoughts -

    ScopedHandle() -> auto-nullize the handle member
    explicit ScopedHandle(Handle) -> construct with a given handle value.
    ScopedHandle(Handle&&) -> move-ctor. transfer ownership.
    ~ScopedHandle() -> release handle if necessary.
    ScopedHandle& operator=(nullptr_t) -> explicit release.
    ScopedHandle& operator=(Handle&&) -> move-assign. transfer ownership.

    Release() -> explicit release.
    Reset() -> replace the handle.
    swap() -> swap handle.
    Close()

    Get()
    operator bool
    operator Handle

    -- non-member --
    std::hash<ScopedHandle>
    std::swap
*/
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

    ScopedHandle& operator=(nullptr_t)
    {
        Close();
        return *this;
    }

    void Close()
    {
        if (Traits::IsValid(handle_)) {
            Traits::Close(handle_);
            handle_ = Traits::NullHandle();
        }
    }

    Handle Get() const
    {
        return handle_;
    }

private:
    Handle handle_;
};

}   // namespace kbase

#endif  // KBASE_MEMORY_SCOPED_HANDLE_H_