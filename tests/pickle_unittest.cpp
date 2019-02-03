/*
 @ 0xCCCCCCCC
*/

#include <functional>
#include <list>
#include <map>
#include <tuple>
#include <vector>

#include "catch2/catch.hpp"

#include "kbase/pickle.h"
#include "kbase/secure_c_runtime.h"

namespace {

using kbase::Pickle;
using kbase::PickleReader;

typedef struct {
    uint32_t payload_size;
} PickleHeader;

const wchar_t kChaosData[] = L"1234";
const size_t kChaosDataSize = sizeof(kChaosData);

auto data_list = std::make_tuple(true,
                                 false,
                                 65535,
                                 0xDEADBEEF,
                                 INT64_C(1),
                                 UINT64_C(0xFFFFFFFFFFFFFFFF),
                                 3.14F,
                                 3.1415926,
                                 std::string("hello"),
                                 std::wstring(L"world"));

void MarshalDataToPickle(Pickle& pk)
{
    pk << (std::get<0>(data_list));
    pk << (std::get<1>(data_list));
    pk << (std::get<2>(data_list));
    pk << (std::get<3>(data_list));
    pk << (std::get<4>(data_list));
    pk << (std::get<5>(data_list));
    pk << (std::get<6>(data_list));
    pk << (std::get<7>(data_list));
    pk << (std::get<8>(data_list));
    pk << (std::get<9>(data_list));
}

auto UnMarshalDataFromPickle(const Pickle& pk)->decltype(data_list)
{
    PickleReader it(pk);
    bool b_data;
    it >> b_data;
    bool b_data2;
    it >> b_data2;
    int n_data;
    it >> n_data;
    uint32_t un_data;
    it >> un_data;
    int64_t n64_data;
    it >> n64_data;
    uint64_t un64_data;
    it >> un64_data;
    float f_data;
    it >> f_data;
    double d_data;
    it >> d_data;
    std::string s_data;
    it >> s_data;
    std::wstring ws_data;
    it >> ws_data;

    return std::tie(b_data, b_data2, n_data, un_data, n64_data, un64_data, f_data,
                    d_data, s_data, ws_data);
}

bool EqualsPickle(const Pickle& lhs, const Pickle& rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    return memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
}

}   // namespace

namespace kbase {

TEST_CASE("Basic semantics", "[Pickle]")
{
    SECTION("constructions")
    {
        Pickle pk;
        REQUIRE(pk.payload_empty());
        MarshalDataToPickle(pk);

        // from serialized buffer.
        std::vector<char> buf;
        buf.resize(pk.size());
        kbase::SecureMemcpy(buf.data(), buf.size(), pk.data(), pk.size());
        Pickle copy_pk(buf.data(), buf.size());
        REQUIRE(EqualsPickle(pk, copy_pk));

        // copy-ctor
        Pickle copy_pickle(pk);
        REQUIRE(EqualsPickle(pk, copy_pickle));

        // move-ctor
        Pickle move_pickle(std::move(copy_pickle));
        REQUIRE(EqualsPickle(pk, move_pickle));
    }

    SECTION("assignments")
    {
        Pickle pickle;
        MarshalDataToPickle(pickle);
        REQUIRE(pickle.size() > 64);

        SECTION("copy assignments")
        {
            // larger to smaller.
            Pickle cp_pickle = pickle;
            REQUIRE(EqualsPickle(pickle, cp_pickle));

            // smaller to larger.
            Pickle unused_pickle;
            cp_pickle = unused_pickle;
            REQUIRE(EqualsPickle(unused_pickle, cp_pickle));
        }

        SECTION("move assignments")
        {
            Pickle another_pickle(pickle);
            REQUIRE_FALSE(another_pickle.payload_empty());
            Pickle brand_new_pickle;
            REQUIRE(brand_new_pickle.payload_empty());
            brand_new_pickle = std::move(another_pickle);
            REQUIRE_FALSE(brand_new_pickle.payload_empty());
            REQUIRE(EqualsPickle(pickle, brand_new_pickle));
        }
    }
}

TEST_CASE("Serializing primitives", "[Pickle]")
{
    Pickle pickle;
    REQUIRE(sizeof(PickleHeader) == pickle.size());
    REQUIRE(pickle.payload_empty());

    uint8_t byte_value = 127;
    pickle.Write(&byte_value, sizeof(uint8_t));
    // No padding for this write.
    REQUIRE(1 == pickle.payload_size());
    const auto* byte_probe = static_cast<const uint8_t*>(pickle.payload());
    REQUIRE(byte_value == *byte_probe);

    short short_value = 32767;
    pickle.Write(&short_value, sizeof(short_value));
    REQUIRE(6 == pickle.payload_size());
    const auto* int_probe = reinterpret_cast<const int*>(pickle.payload());
    REQUIRE(0x7F == *int_probe);

    pickle.Write(kChaosData, kChaosDataSize);
#if defined(OS_WIN)
    REQUIRE(18 == pickle.payload_size());
#else
    REQUIRE(28 == pickle.payload_size());   // wchar_t on posix-platform is in 4-byte
#endif
    const auto* char_probe = reinterpret_cast<const char*>(pickle.payload()) + 8;
    REQUIRE('1' == *char_probe);
}

TEST_CASE("Serializing strings", "[Pickle]")
{
    Pickle pickle;
    std::wstring str = L"abcde";
    pickle << str;
#if defined(OS_WIN)
    REQUIRE(18 == pickle.payload_size());
#else
    REQUIRE(28 == pickle.payload_size());   // wchar_t on posix-platform is in 4-byte
#endif
    const size_t* size_probe = reinterpret_cast<const size_t*>(pickle.payload());
    REQUIRE(5 == *size_probe);
    const wchar_t* str_probe = reinterpret_cast<const wchar_t*>(size_probe + 1);
    std::wstring saved_str(str_probe, 5);
    REQUIRE(str == saved_str);
}

TEST_CASE("Flexibilities of reader", "[Pickle]")
{
    SECTION("reader can validate buffer")
    {
        {
            Pickle pickle;
            pickle << true << 128 << std::string("abcd");
            PickleReader reader(pickle.data(), pickle.size());
            REQUIRE(!!reader);
        }
        {
            Pickle pickle;
            PickleReader reader(pickle);
            REQUIRE_FALSE(!!reader);
        }
    }

    SECTION("reader can skip data")
    {
        Pickle pickle;
        const std::string str("abc");
        pickle << true << 128 << str;
        PickleReader reader(pickle.data(), pickle.size());
        REQUIRE(!!reader);
        reader.SkipData(sizeof(bool));
        reader.SkipData(sizeof(int));
        REQUIRE(!!reader);
        reader.SkipData(sizeof(size_t));
        reader.SkipData(sizeof(char) * str.length());
        REQUIRE_FALSE(!!reader);
    }

    SECTION("normal readings")
    {
        Pickle pickle;
        const std::string str("abc");
        pickle << true << 128 << str;
        {
            PickleReader reader(pickle);
            REQUIRE(!!reader);
            bool boolean;
            int num;
            std::string s;
            reader >> boolean >> num >> s;
            REQUIRE(true == boolean);
            REQUIRE(128 == num);
            REQUIRE(str == s);
        }
    }
}

TEST_CASE("Support of several complex containers in STL", "[Pickle]")
{
    SECTION("empty string")
    {
        std::string s;
        Pickle pickle;
        pickle << s;
        REQUIRE(8 == pickle.payload_size());
        PickleReader reader(pickle);
        std::string ss;
        reader >> ss;
        REQUIRE(s == ss);
    }

    SECTION("vector")
    {
        Pickle pickle;
        std::vector<int> vi {1, 3, 5};
        std::vector<std::string> vs { "hello", "world" };
        pickle << vi << vs;
        PickleReader reader(pickle);
        std::vector<int> vv;
        std::vector<std::string> vvs;
        reader >> vv >> vvs;
        REQUIRE(vi == vv);
        REQUIRE(vs == vvs);
    }

    SECTION("empty vector")
    {
        std::vector<std::string> vs;
        Pickle pickle;
        pickle << vs;
        std::vector<std::string> cvs;
        PickleReader reader(pickle);
        reader >> cvs;
        REQUIRE(vs == cvs);
    }

    SECTION("list")
    {
        Pickle pickle;
        std::list<int> il { 1, 3, 5, 7 };
        pickle << il;
        std::list<int> cil;
        PickleReader reader(pickle);
        reader >> cil;
        REQUIRE(il == cil);
    }

    SECTION("list of vectors")
    {
        Pickle pickle;
        std::list<std::vector<std::string>> lies {
            {"hello", "world"},
            {"kc", "is", "a genius"}
        };
        pickle << lies;
        PickleReader reader(pickle);
        decltype(lies) truth;
        reader >> truth;
        REQUIRE(lies == truth);
    }

    SECTION("pair")
    {
        Pickle pickle;
        auto p1 = std::make_pair(3.14, 127);
        auto p2 = std::make_pair(std::string("vala"), 111);
        pickle << p1 << p2;
        PickleReader reader(pickle);
        decltype(p1) cp1;
        reader >> cp1;
        REQUIRE(p1 == cp1);
        decltype(p2) cp2;
        reader >> cp2;
        REQUIRE(p2 == cp2);
        REQUIRE_FALSE(!!reader);
    }

    SECTION("set")
    {
        Pickle pickle;
        std::set<std::string, std::greater<std::string>> ss { "hello", "world", "pickle", "test" };
        pickle << ss;
        PickleReader reader(pickle);
        decltype(ss) css;
        reader >> css;
        REQUIRE(ss == css);
        REQUIRE_FALSE(!!reader);
    }

    SECTION("map")
    {
        Pickle pickle;
        std::map<std::string, int> table {
            {"hello", 5}, {"world", 5}, {"test", 4}
        };
        pickle << table;
        PickleReader reader(pickle);
        decltype(table) ct;
        reader >> ct;
        REQUIRE(table == ct);
        REQUIRE_FALSE(!!reader);
    }

    SECTION("unordered_set")
    {
        Pickle pickle;
        std::unordered_set<std::string> uss { "hello", "world", "pickle", "test" };
        pickle << uss;
        PickleReader reader(pickle);
        decltype(uss) cuss;
        reader >> cuss;
        REQUIRE(uss == cuss);
        REQUIRE_FALSE(!!reader);
    }

    SECTION("unordered_map")
    {
        Pickle pickle;
        std::unordered_map<std::string, int> utable {
            {"hello", 5}, {"world", 5}, {"test", 4}
        };
        pickle << utable;
        PickleReader reader(pickle);
        decltype(utable) cut;
        reader >> cut;
        REQUIRE(utable == cut);
        REQUIRE_FALSE(!!reader);
    }
}

TEST_CASE("General data serialization and deserialization", "[Pickle]")
{
    Pickle pickle;
    MarshalDataToPickle(pickle);
    auto unmarshalled_data_list = UnMarshalDataFromPickle(pickle);
    REQUIRE(unmarshalled_data_list == data_list);
}

}   // namespace kbase
