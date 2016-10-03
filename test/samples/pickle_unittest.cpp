/*
 @ 0xCCCCCCCC
*/

#include <functional>
#include <list>
#include <map>
#include <tuple>
#include <vector>

#include "gtest/gtest.h"

#include "kbase/pickle.h"

using kbase::Pickle;
using kbase::PickleReader;

namespace {

typedef struct {
    uint32_t payload_size;
} PickleHeader;

const wchar_t kChaosData[] = L"1234";
const size_t kChaosDataSize = sizeof(kChaosData);

auto data_list = std::make_tuple(true,
                                 false,
                                 65535,
                                 0xDEADBEEF,
                                 1LL,
                                 0xFFFFFFFFFFFFFFFFULL,
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

TEST(PickleTest, Construction)
{
    Pickle pk;
    EXPECT_TRUE(pk.payload_empty());
    MarshalDataToPickle(pk);

    // from serialized buffer.
    std::vector<char> buf;
    buf.resize(pk.size());
    memcpy_s(buf.data(), buf.size(), pk.data(), pk.size());
    Pickle copy_pk(buf.data(), buf.size());
    EXPECT_TRUE(EqualsPickle(pk, copy_pk));

    // copy-ctor
    Pickle copy_pickle(pk);
    EXPECT_TRUE(EqualsPickle(pk, copy_pickle));

    // move-ctor
    Pickle move_pickle(std::move(copy_pickle));
    EXPECT_TRUE(EqualsPickle(pk, move_pickle));
}

TEST(PickleTest, Assignments)
{
    Pickle pickle;
    MarshalDataToPickle(pickle);
    ASSERT_TRUE(pickle.size() > 64);

    // copy-assignment
    {
        // larger to smaller.
        Pickle cp_pickle;
        cp_pickle = pickle;
        EXPECT_TRUE(EqualsPickle(pickle, cp_pickle));

        // smaller to larger.
        Pickle unused_pickle;
        cp_pickle = unused_pickle;
        EXPECT_TRUE(EqualsPickle(unused_pickle, cp_pickle));
    }

    // move-assignment
    {
        Pickle another_pickle(pickle);
        EXPECT_FALSE(another_pickle.payload_empty());
        Pickle brand_new_pickle;
        EXPECT_TRUE(brand_new_pickle.payload_empty());
        brand_new_pickle = std::move(another_pickle);
        EXPECT_FALSE(brand_new_pickle.payload_empty());
        EXPECT_TRUE(EqualsPickle(pickle, brand_new_pickle));
    }
}

TEST(PickleTest, FundamentalWrite)
{
    Pickle pickle;
    EXPECT_EQ(sizeof(PickleHeader), pickle.size());
    EXPECT_TRUE(pickle.payload_empty());

    uint8_t byte_value = 127;
    pickle.Write(&byte_value, sizeof(uint8_t));
    // No padding for this write.
    EXPECT_EQ(1, pickle.payload_size());
    const auto* byte_probe = static_cast<const uint8_t*>(pickle.payload());
    EXPECT_EQ(byte_value, *byte_probe);

    short short_value = 32767;
    pickle.Write(&short_value, sizeof(short_value));
    EXPECT_EQ(6, pickle.payload_size());
    const auto* int_probe = reinterpret_cast<const int*>(pickle.payload());
    EXPECT_EQ(0x7F, *int_probe);

    pickle.Write(kChaosData, kChaosDataSize);
    EXPECT_EQ(18, pickle.payload_size());
    const auto* char_probe = reinterpret_cast<const char*>(pickle.payload()) + 8;
    EXPECT_EQ('1', *char_probe);
}

TEST(PickleTest, SerializeString)
{
    Pickle pickle;
    std::wstring str = L"abcde";
    pickle << str;
    EXPECT_EQ(18, pickle.payload_size());
    const size_t* size_probe = reinterpret_cast<const size_t*>(pickle.payload());
    EXPECT_EQ(5, *size_probe);
    const wchar_t* str_probe = reinterpret_cast<const wchar_t*>(size_probe + 1);
    std::wstring saved_str(str_probe, 5);
    EXPECT_EQ(str, saved_str);
}

TEST(PickleTest, ReaderValidaty)
{
    {
        Pickle pickle;
        pickle << true << 128 << std::string("abcd");
        PickleReader reader(pickle.data(), pickle.size());
        EXPECT_TRUE(!!reader);
    }
    {
        Pickle pickle;
        PickleReader reader(pickle);
        EXPECT_FALSE(!!reader);
    }
}

TEST(PickleTest, ReaderSkipData)
{
    Pickle pickle;
    const std::string str("abc");
    pickle << true << 128 << str;
    PickleReader reader(pickle.data(), pickle.size());
    EXPECT_TRUE(!!reader);
    reader.SkipData(sizeof(bool));
    reader.SkipData(sizeof(int));
    EXPECT_TRUE(!!reader);
    reader.SkipData(sizeof(size_t));
    reader.SkipData(sizeof(char) * str.length());
    EXPECT_FALSE(!!reader);
}

TEST(PickleTest, ReaderRead)
{
    Pickle pickle;
    const std::string str("abc");
    pickle << true << 128 << str;
    {
        PickleReader reader(pickle);
        ASSERT_TRUE(!!reader);
        bool boolean;
        int num;
        std::string s;
        reader >> boolean >> num >> s;
        EXPECT_EQ(true, boolean);
        EXPECT_EQ(128, num);
        EXPECT_EQ(str, s);
    }
}

TEST(PickleTest, EmptyString)
{
    std::string s;
    Pickle pickle;
    pickle << s;
    EXPECT_EQ(8, pickle.payload_size());
    PickleReader reader(pickle);
    std::string ss;
    reader >> ss;
    EXPECT_EQ(s, ss);
}

TEST(PickleTest, ContainerVector)
{
    {
        Pickle pickle;
        std::vector<int> vi {1, 3, 5};
        std::vector<std::string> vs { "hello", "world" };
        pickle << vi << vs;
        PickleReader reader(pickle);
        std::vector<int> vv;
        std::vector<std::string> vvs;
        reader >> vv >> vvs;
        EXPECT_EQ(vi, vv);
        EXPECT_EQ(vs, vvs);
    }
    {
        std::vector<std::string> vs;
        Pickle pickle;
        pickle << vs;
        std::vector<std::string> cvs;
        PickleReader reader(pickle);
        reader >> cvs;
        EXPECT_EQ(vs, cvs);
    }
}

TEST(PickleTest, ContainerList)
{
    {
        Pickle pickle;
        std::list<int> il { 1, 3, 5, 7 };
        pickle << il;
        std::list<int> cil;
        PickleReader reader(pickle);
        reader >> cil;
        EXPECT_EQ(il, cil);
    }
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
        EXPECT_EQ(lies, truth);
    }
}

TEST(PickleTest, ContainerPair)
{
    Pickle pickle;
    auto p1 = std::make_pair(3.14, 127);
    auto p2 = std::make_pair(std::string("vala"), 111);
    pickle << p1 << p2;
    PickleReader reader(pickle);
    decltype(p1) cp1;
    reader >> cp1;
    EXPECT_EQ(p1, cp1);
    decltype(p2) cp2;
    reader >> cp2;
    EXPECT_EQ(p2, cp2);
    EXPECT_FALSE(!!reader);
}

TEST(PickleTest, ContainerSet)
{
    Pickle pickle;
    std::set<std::string, std::greater<std::string>> ss { "hello", "world", "pickle", "test" };
    pickle << ss;
    PickleReader reader(pickle);
    decltype(ss) css;
    reader >> css;
    EXPECT_EQ(ss, css);
    EXPECT_FALSE(!!reader);
}

TEST(PickleTest, ContainerMap)
{
    Pickle pickle;
    std::map<std::string, int> table {
        {"hello", 5}, {"world", 5}, {"test", 4}
    };
    pickle << table;
    PickleReader reader(pickle);
    decltype(table) ct;
    reader >> ct;
    EXPECT_EQ(table, ct);
    EXPECT_FALSE(!!reader);
}

TEST(PickleTest, ContainerUnorderedSet)
{
    Pickle pickle;
    std::unordered_set<std::string> uss { "hello", "world", "pickle", "test" };
    pickle << uss;
    PickleReader reader(pickle);
    decltype(uss) cuss;
    reader >> cuss;
    EXPECT_EQ(uss, cuss);
    EXPECT_FALSE(!!reader);
}

TEST(PickleTest, ContainerUnorderedMap)
{
    Pickle pickle;
    std::unordered_map<std::string, int> utable {
        {"hello", 5}, {"world", 5}, {"test", 4}
    };
    pickle << utable;
    PickleReader reader(pickle);
    decltype(utable) cut;
    reader >> cut;
    EXPECT_EQ(utable, cut);
    EXPECT_FALSE(!!reader);
}

TEST(PickleTest, SerializeAndDeserialize)
{
    Pickle pickle;
    MarshalDataToPickle(pickle);
    auto unmarshalled_data_list = UnMarshalDataFromPickle(pickle);
    EXPECT_EQ(unmarshalled_data_list, data_list);
}