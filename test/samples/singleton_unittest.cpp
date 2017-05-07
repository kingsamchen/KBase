/*
 @ 0xCCCCCCCC
*/

#include <atomic>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

#include "kbase/singleton.h"
#include "kbase/error_exception_util.h"

namespace {

struct DataPack {
    int i;
    double d;
};

std::atomic<DataPack*> g_data_pack_ptr { nullptr };

class SingletonUser {
public:
    static SingletonUser* Instance()
    {
        return kbase::Singleton<SingletonUser>::instance();
    }

private:
    friend kbase::DefaultSingletonTraits<SingletonUser>;

    SingletonUser()
        : n(123)
    {}

    ~SingletonUser()
    {
        std::cout << "~SingletonUser()\n";
    }

public:
    int n;
};

class LeakySingletonUser {
public:
    static LeakySingletonUser* instance()
    {
        return kbase::Singleton<LeakySingletonUser,
                                kbase::LeakySingletonTraits<LeakySingletonUser>>::instance();
    }

    void Greet()
    {
        FORCE_AS_MEMBER_FUNCTION();
        std::cout << "hello there\n";
    }

private:
    LeakySingletonUser() = default;

    ~LeakySingletonUser()
    {
        // Will never be executed.
        EXPECT_TRUE(kbase::NotReached());
    }

    friend kbase::LeakySingletonTraits<LeakySingletonUser>;
};

class CustomSingleton {
public:
    struct CustomSingletonTraits {
        static constexpr bool kDestroyAtExit = false;

        static CustomSingleton* Create()
        {
            return new CustomSingleton("hello from CustomSingleton");
        }
    };

    static CustomSingleton* instance()
    {
        return kbase::Singleton<CustomSingleton, CustomSingletonTraits>::instance();
    }

    void DumpMessage() const
    {
        std::cout << msg_;
    }

private:
    explicit CustomSingleton(std::string msg)
        : msg_(std::move(msg))
    {}

    ~CustomSingleton() = default;

private:
    std::string msg_;
};

}   // namespace

TEST(SingletonTest, DefaultTraits)
{
    kbase::AtExitManager exit_manager;
    auto i = kbase::Singleton<int>::instance();
    *i = 0x12345678;

    EXPECT_EQ(123, SingletonUser::Instance()->n);
}

TEST(SingletonTest, Leaky)
{
    LeakySingletonUser::instance()->Greet();
}

TEST(SingletonTest, CustomTraits)
{
    CustomSingleton::instance()->DumpMessage();
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
