/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_STACK_WALKER_H_
#define KBASE_STACK_WALKER_H_

#include <array>

struct _CONTEXT;
using CONTEXT = _CONTEXT;

namespace kbase {

class StackWalker {
public:
    StackWalker();

    // Using in SEH context.
    explicit StackWalker(CONTEXT* context);

    ~StackWalker() = default;

    void DumpCallStack(std::ostream& stream);

    std::string CallStackToString();

private:
    static const size_t kMaxStackFrames = 64;
    std::array<void*, kMaxStackFrames> stack_frames_ { nullptr };
    size_t valid_frame_count_ = 0;
};

}   // namespace kbase

#endif  // KBASE_STACK_WALKER_H_