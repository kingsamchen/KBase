
#include <cstdio>
#include <conio.h>
#include <iostream>
#include <memory>

#include "Pickle.h"

using std::cout;
using std::endl;

int main(int /*argc*/, char* /*argv[]*/)
{
    std::string s = "hello world";

    KBase::Pickle pickle;
    // serialize
    pickle.WriteInt(0x1234);
    pickle.WriteBool(true);
    pickle.WriteDouble(3.14159);
    pickle.WriteString("hell world");

    std::cout << "marshal data complete" << std::endl;

    // transmit serialized data if necessary
    size_t buffer_size = pickle.size();
    std::unique_ptr<char[]> buff(new char[buffer_size]);
    memcpy(buff.get(), pickle.data(), buffer_size);

    KBase::Pickle pickle_out(buff.get(), buffer_size);
    
    int protocol = 0;
    bool flag = false;
    double pi = 0.0;
    std::string str;

    // deserialize
    KBase::PickleIterator it(pickle_out);
    it.ReadInt(&protocol);
    it.ReadBool(&flag);
    it.ReadDouble(&pi);
    it.ReadString(&str);

    cout << "unmarshal:" << protocol << endl  << flag << endl << pi << endl << str;

    _getch();
    return 0;
}

#ifdef _DEBUG
namespace KBase {
//void _pickle_dump(const Pickle& pk)
//{
//    cout << "capacity:" << pk.capacity_ << endl
//         << "header:" << std::hex << pk.header_ << endl
//         << "payload addr:" << reinterpret_cast<int>(pk.payload()) << endl << std::dec
//         << "payload size:" << pk.header_->payload_size << endl
//         << "buffer_offset:" << pk.buffer_offset_ << endl;
//}
}
#endif