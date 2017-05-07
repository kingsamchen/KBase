/*
 @ 0xCCCCCCCC
*/

#include <functional>
#include <string>

#include "gtest/gtest.h"

#include "kbase/signals.h"

namespace {

void DoWork(int id, std::string&& msg)
{
    std::cout << "Dowrk -> " << id << ":" << msg << std::endl;
}

class Foo {
public:
    explicit Foo(std::string tag)
        : tag_(std::move(tag))
    {}

    ~Foo() = default;

    static void S_DoWork(int id, std::string&& msg)
    {
        std::cout << "Foo::S_DoWork -> " << id << ":" << msg << std::endl;
    }

    void DoWork(int id, std::string&& msg) const
    {
        std::cout << "Foo::DoWork -> [" << id << "-" << tag_ << "]:" << msg << std::endl;
    }

private:
    std::string tag_;
};

}

TEST(SignalsTest, ConnectToFunctions)
{
    using namespace std::placeholders;

    Foo foo("kc");
    kbase::Signal<int, std::string&&> worker_signal;
    worker_signal.Connect(&DoWork);
    worker_signal.Connect(&Foo::S_DoWork);
    worker_signal.Connect(std::bind(&Foo::DoWork, &foo, _1, _2));

    worker_signal.Emit(1024, "hello world");
    worker_signal.DisconnectAll();
}

TEST(SignalsTest, SlotsAndDisconnect)
{
    // In order to destroy signal manually.
    auto worker_signal = std::make_unique <kbase::Signal<int, std::string&&>>();
    auto slot_normal = worker_signal->Connect(&DoWork);
    auto slot_foo = worker_signal->Connect(&Foo::S_DoWork);

    std::cout << "When all are prepared\n";
    worker_signal->Emit(123, "Hail Hydra");

    slot_foo.Disconnect();
    std::cout << "Disconnect with Foo::S_DoWork\n";
    worker_signal->Emit(123, "Hail Hydra");

    worker_signal = nullptr;
    std::cout << "Now we destroy signal and then call Disconnect on the left slot\n";
    slot_normal.Disconnect();
}

TEST(SignalsTest, EmitWhenObjectAreGone)
{
    using namespace std::placeholders;
    kbase::Signal<int, std::string&&> worker_signal;
    auto foo = std::make_shared<Foo>("kfc");
    worker_signal.Connect(std::bind(&Foo::DoWork, foo.get(), _1, _2), foo);
    worker_signal.Emit(111, "You're an asshole\n");

    foo = nullptr;
    worker_signal.Emit(111, "Are you alive\n");
}
