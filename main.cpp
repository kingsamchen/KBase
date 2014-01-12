
#include <cstdio>
#include <conio.h>
#include <iostream>
#include <memory>

#include "Pickle.h"

using std::cout;
using std::endl;
using KBase::Pickle;
using KBase::_dump;

int main(int /*argc*/, char* /*argv[]*/)
{
    std::string s = "hello world";
    Pickle pickle;
    pickle.WriteInt(0x1234);
    pickle.WriteBool(true);
    pickle.WriteDouble(3.14159);

    cout << "marshal data complete" << endl;

    int protocol = 0;
    bool flag = false;
    double pi = 0.0;

    KBase::PickleIterator it(pickle);
    it.ReadInt(&protocol);
    it.ReadBool(&flag);
    it.ReadDouble(&pi);

    cout << "unmarshal:" << protocol << " " << flag << " " << pi << endl;

    _getch();
    return 0;
}

#ifdef _DEBUG
namespace KBase {
void _dump(const Pickle& pk)
{
    cout << "capacity:" << pk.capacity_ << endl
         << "header:" << std::hex << pk.header_ << endl
         << "payload addr:" << reinterpret_cast<int>(pk.payload()) << endl << std::dec
         << "payload size:" << pk.header_->payload_size << endl
         << "buffer_offset:" << pk.buffer_offset_ << endl;
}
}
#endif