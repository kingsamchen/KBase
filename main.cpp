
#include <cstdio>
#include <conio.h>
#include <iostream>
#include <memory>
#include <string>

#include "auto_reset.h"

using std::cout;
using std::endl;

int main(int /*argc*/, char* /*argv[]*/)
{
    std::string str("hello world");
    
    {
        cout << "stage 1:" << str << endl;
        KBase::AutoReset<std::string> auto_str(&str, "kingsamchen");
        cout << "stage 2:" << str << endl;
    }

    cout << "stage 3:" << str << endl;

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