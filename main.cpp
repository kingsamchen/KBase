
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
    pickle.WriteString(s);
    pickle.WriteInt(123);
    char* p = static_cast<char*>(const_cast<void*>(pickle.data()));
    cout << *(int*)p << endl;
    p += 4;
    int len = *(int*)p;
    p += 4;
    cout << len << endl;
    std::string st(p, p + len);
    cout << st;
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