/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include "kbase/endian_utils.h"

namespace {

template<typename T>
struct EndianPair {
    T le;
    T be;
};

EndianPair<unsigned short> usp {uint16_t(0x1234), uint16_t(0x3412)};
// 0xCCFF -> 0xFFCC
EndianPair<short> sp {int16_t(-13057), int16_t(-52)};

EndianPair<unsigned int> uip {0x12345678U, 0x78563412U};
EndianPair<int> ip {0x7FCCDEAD, int(0xADDECC7F)};

EndianPair<uint64_t> ullp {uint64_t(0xDEADBEEFCCDDFFEE), uint64_t(0xEEFFDDCCEFBEADDE)};
EndianPair<int64_t> llp {int64_t(0x12345678ABCD00FF), int64_t(0xFF00CDAB78563412)};

}   // namespace

namespace kbase {

TEST_CASE("Little to big", "[EndianUtils]")
{
    SECTION("for 16-bit integers") {
        CHECK(HostToNetwork(usp.le) == usp.be);
        CHECK(HostToNetwork(sp.le) == sp.be);
    }

    SECTION("for 32-bit integers") {
        CHECK(HostToNetwork(uip.le) == uip.be);
        CHECK(HostToNetwork(ip.le) == ip.be);
    }

    SECTION("for 64-bit integers") {
        CHECK(HostToNetwork(ullp.le) == ullp.be);
        CHECK(HostToNetwork(llp.le) == llp.be);
    }
}

TEST_CASE("Big to little", "[EndianUtils]")
{
    SECTION("for 16-bit integers") {
        CHECK(NetworkToHost(usp.be) == usp.le);
        CHECK(NetworkToHost(sp.be) == sp.le);
    }

    SECTION("for 32-bit integers") {
        CHECK(NetworkToHost(uip.be) == uip.le);
        CHECK(NetworkToHost(ip.be) == ip.le);
    }

    SECTION("for 64-bit integers") {
        CHECK(NetworkToHost(ullp.be) == ullp.le);
        CHECK(NetworkToHost(llp.be) == llp.le);
    }
}

}   // namespace kbase
