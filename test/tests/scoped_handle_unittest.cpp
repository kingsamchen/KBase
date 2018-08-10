/*
 @ 0xCCCCCCCC
*/

#include <vector>

#include "catch2/catch.hpp"

#include "kbase/scoped_handle.h"

namespace kbase {

TEST_CASE("General class traits", "[ScopedHandle]")
{
    SECTION("Normal")
    {
        {
            ScopedWinHandle sys_h;
            REQUIRE_FALSE(sys_h);
        }

        {
            ScopedWinHandle event_h(CreateEventW(nullptr, TRUE, TRUE, nullptr));
            REQUIRE(!!event_h);
            INFO("event_h underlying handle: " << event_h.get() << "\n");
        }
    }

    SECTION("test and nullize")
    {
        ScopedWinHandle event_h(CreateEventW(nullptr, TRUE, TRUE, nullptr));
        if (!event_h) {
            REQUIRE(false);
        }
        auto h = event_h.release();
        REQUIRE_FALSE(static_cast<bool>(event_h));
        REQUIRE(h != nullptr);
        event_h = nullptr;
        event_h.reset(h);
        REQUIRE(static_cast<bool>(event_h) != false);
        event_h = nullptr;
        REQUIRE_FALSE(static_cast<bool>(event_h));
    }

    SECTION("handle is movable")
    {
        {
            ScopedWinHandle event_h(CreateEventW(nullptr, TRUE, TRUE, nullptr));
            REQUIRE(static_cast<bool>(event_h));
            ScopedWinHandle another_h(CreateEventW(nullptr, TRUE, TRUE, nullptr));
            REQUIRE(static_cast<bool>(event_h));
            REQUIRE(event_h.get() != another_h.get());

            auto reserved_h = event_h.get();
            auto reserved_another_h = another_h.get();
            event_h = std::move(another_h);

            REQUIRE_FALSE(static_cast<bool>(another_h));
            REQUIRE(static_cast<bool>(event_h));
            REQUIRE(reserved_another_h == event_h.get());
            BOOL rv = CloseHandle(reserved_h);
            DWORD err = GetLastError();
            REQUIRE_FALSE(rv);
            REQUIRE(err == ERROR_INVALID_HANDLE);
        }

        {
            std::vector<ScopedWinHandle> events;
            for (int i = 0; i < 10; ++i) {
                events.emplace_back(CreateEventW(nullptr, TRUE, TRUE, nullptr));
            }
        }
    }
}

}   // namespace kbase
