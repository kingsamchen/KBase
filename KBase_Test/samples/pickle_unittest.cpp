
#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\pickle.h"

#include <tuple>
#include <vector>

using kbase::Pickle;
using kbase::PickleIterator;

namespace {

typedef struct {
    uint32_t payload_size;
} PickleHeader;

const wchar_t kChaosData[] = L"helloworld\u1234\u4321\u9876\u5678";
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

void MarshalDataToPickle(Pickle* pk)
{
    ASSERT_TRUE(pk->empty());

    ASSERT_TRUE(pk->Write(std::get<0>(data_list)));
    ASSERT_TRUE(pk->Write(std::get<1>(data_list)));
    ASSERT_TRUE(pk->Write(std::get<2>(data_list)));
    ASSERT_TRUE(pk->Write(std::get<3>(data_list)));
    ASSERT_TRUE(pk->Write(std::get<4>(data_list)));
    ASSERT_TRUE(pk->Write(std::get<5>(data_list)));
    ASSERT_TRUE(pk->Write(std::get<6>(data_list)));
    ASSERT_TRUE(pk->Write(std::get<7>(data_list)));
    ASSERT_TRUE(pk->Write(std::get<8>(data_list)));
    ASSERT_TRUE(pk->Write(std::get<9>(data_list)));
}

auto UnMarshalDataFromPickle(const Pickle& pk)->decltype(data_list)
{
    PickleIterator it(pk);
    bool b_data;
    it.Read(&b_data);
    bool b_data2;
    it.Read(&b_data2);
    int n_data;
    it.Read(&n_data);
    uint32_t un_data;
    it.Read(&un_data);
    int64_t n64_data;
    it.Read(&n64_data);
    uint64_t un64_data;
    it.Read(&un64_data);
    float f_data;
    it.Read(&f_data);
    double d_data;
    it.Read(&d_data);
    std::string s_data;
    it.Read(&s_data);
    std::wstring ws_data;
    it.Read(&ws_data);

    return std::tie(b_data, b_data2, n_data, un_data, n64_data, un64_data, f_data,
                    d_data, s_data, ws_data);
}

}   // namespace

TEST(PickleTest, WriteBytesAndDataSize)
{
    Pickle pickle;
    EXPECT_EQ(sizeof(PickleHeader), pickle.size());
    EXPECT_TRUE(pickle.empty());
    
    ASSERT_TRUE(pickle.WriteByte(kChaosData, kChaosDataSize));
    EXPECT_EQ(sizeof(PickleHeader) + kChaosDataSize, pickle.size());
    EXPECT_FALSE(pickle.empty());
    EXPECT_EQ(kChaosDataSize, pickle.payload_size());
}

TEST(PickleTest, SerializeAndDeserialize)
{
    Pickle pickle;
    MarshalDataToPickle(&pickle);
    auto unmarshalled_data_list = UnMarshalDataFromPickle(pickle);
    EXPECT_EQ(unmarshalled_data_list, data_list);
}

TEST(PickleTest, Construction)
{
    Pickle pk;
    MarshalDataToPickle(&pk);

    // holds in buffer.
    std::vector<char> buf;
    buf.resize(pk.size());
    memcpy_s(buf.data(), buf.size(), pk.data(), pk.size());

    // const pickle
    Pickle const_pk(buf.data(), buf.size());
    auto ret_from_const_pk = UnMarshalDataFromPickle(const_pk);
    EXPECT_EQ(data_list, ret_from_const_pk);

    // deep-copy from copy-ctor
    Pickle copy_pickle = pk;
    auto ret_from_copy_ctor = UnMarshalDataFromPickle(copy_pickle);
    EXPECT_EQ(data_list, ret_from_copy_ctor);

    // deep-copy from copy-assignment
    Pickle assigned_pickle;
    assigned_pickle = const_pk;
    auto ret_from_assigned = UnMarshalDataFromPickle(assigned_pickle);
    EXPECT_EQ(data_list, ret_from_assigned);
}

TEST(PickleTest, Move)
{

}