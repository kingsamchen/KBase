/*
 @ 0xCCCCCCCC
*/

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#include "catch2/catch.hpp"

#include "kbase/lazy.h"

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

namespace kbase {

TEST_CASE("Construction is delayed until first use", "[Lazy]")
{
    {
        kbase::Lazy<DataPack> data;
        REQUIRE(g_activation_trigger == false);
        REQUIRE(data.value().inited);
        REQUIRE(g_activation_trigger == true);
    }

    REQUIRE(g_activation_trigger == false);
}

TEST_CASE("Use customized contructor", "[Lazy]")
{
    {
        kbase::Lazy<DataPack> data([]() { return new DataPack("hello world"); });
        REQUIRE(g_activation_trigger == false);
        REQUIRE(data.value().msg == "hello world");
        REQUIRE(g_activation_trigger == true);
    }

    REQUIRE(g_activation_trigger == false);
}

TEST_CASE("Construction of lazy is thread-safe", "[Lazy]")
{
    REQUIRE(g_activation_trigger == false);
    std::vector<std::thread> vth;
    for (int i = 0; i < 100; ++i) {
        vth.emplace_back([](int id) {
            DataPack* ptr = &MakeDataPackDeferred();
            auto global_ptr = g_dp_ptr.load();
            if (!global_ptr) {
                g_dp_ptr.store(ptr);
                std::cout << "thread " << id << " activated datapack" << std::endl;
            } else {
                REQUIRE(ptr == global_ptr);
            }
            REQUIRE(ptr->inited);
        }, i);

        vth.back().join();
    }

    REQUIRE(g_activation_trigger == true);
}

}   // namespace kbase
