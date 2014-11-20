/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\memory\lazy.h"

#include <atomic>
#include <string>
#include <thread>
#include <vector>

namespace {

bool g_activation_trigger = false;

void MakeActivationTrigger(bool flag)
{
    g_activation_trigger = flag;
}

struct DataPack {
    DataPack()
    {
        std::cout << "default initialized" << std::endl;
        MakeActivationTrigger(true);
    }

    DataPack(const std::string str)
        : msg(str)
    {
        std::cout << "parameterized initialized" << std::endl;
        MakeActivationTrigger(true);
    }

    ~DataPack()
    {
        MakeActivationTrigger(false);
    }

    std::string msg;
    bool inited = true;
};

DataPack& MakeDataPackDeferred()
{
    static kbase::Lazy<DataPack> s_data_pack;
    return s_data_pack.value();
}

std::atomic<DataPack*> g_dp_ptr {nullptr};

}   // namespace

TEST(LazyTest, Lazyness)
{
    {
        kbase::Lazy<DataPack> data;
        EXPECT_EQ(g_activation_trigger, false);
        EXPECT_TRUE(data.value().inited);
        EXPECT_EQ(g_activation_trigger, true);
    }

    EXPECT_EQ(g_activation_trigger, false);
}

TEST(LazyTest, ControllingConstruction)
{
    {
        kbase::Lazy<DataPack> data([]() { return new DataPack("hello world"); });
        EXPECT_EQ(g_activation_trigger, false);
        EXPECT_STREQ(data.value().msg.c_str(), "hello world");
        EXPECT_EQ(g_activation_trigger, true);
    }

    EXPECT_EQ(g_activation_trigger, false);
}

TEST(LazyTest, ThreadSafety)
{
    EXPECT_EQ(g_activation_trigger, false);
    std::vector<std::thread> vth;
    for (int i = 0; i < 100; ++i) {
        vth.emplace_back([](int id) {
            DataPack* ptr = &MakeDataPackDeferred();
            auto global_ptr = g_dp_ptr.load();
            if (!global_ptr) {
                g_dp_ptr.store(ptr);
                std::cout << "thread " << id << " activated datapack" << std::endl;
            } else {
                EXPECT_EQ(ptr, global_ptr);
            }
            EXPECT_TRUE(ptr->inited);
        }, i);

        vth.back().join();
    }
    
    EXPECT_EQ(g_activation_trigger, true);
}