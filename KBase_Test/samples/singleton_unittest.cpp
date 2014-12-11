/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\memory\singleton.h"

#include <atomic>
#include <thread>
#include <vector>

namespace {

struct DataPack {
    int i;
    double d;
};

std::atomic<DataPack*> g_data_pack_ptr {nullptr};

class SingletonUser {
public:
    static SingletonUser* Instance()
    {
        return kbase::Singleton<SingletonUser>::instance();
    }

private:
    friend kbase::DefaultSingletonTraits<SingletonUser>;
    SingletonUser() {}
    ~SingletonUser() {}

public:
    int n;
};

}   // namespace

TEST(SingletonTest, Valid)
{
    kbase::AtExitManager exit_manager;
    auto i = kbase::Singleton<int>::instance();
    *i = 0x12345678;
}

TEST(SingletonTest, Leaky)
{
    int* i = kbase::Singleton<int, kbase::LeakySingletonTraits<int>>::instance();
    *i = 0x12345678;
    delete i;
}

TEST(SingletonTest, Uniqueness)
{
    kbase::AtExitManager exit_manager;
    std::vector<std::thread> vth;
    for (int i = 0; i < 100; ++i) {
        vth.emplace_back([](int id) {
            auto ptr = kbase::Singleton<DataPack>::instance();
            auto g_ptr = g_data_pack_ptr.load();
            if (!g_ptr) {
                std::cout << "thread " << id << " obtains singleton object first\n";
                g_data_pack_ptr.store(ptr);
            } else {
                EXPECT_EQ(ptr, g_ptr);
            }
        }, i);
        vth.back().join();
    }
}