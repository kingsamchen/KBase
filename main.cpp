
#include <cassert>
#include <cstdio>
#include <conio.h>
#include <iostream>
#include <memory>
#include <string>

#include "strings\string_piece.h"

using std::cout;
using std::endl;

int main(int /*argc*/, char* /*argv[]*/)
{
    KBase::StringPiece sp("this is my new stringpiece");
    std::string s("this is my new stringpiece");

    assert(sp.rfind("new") == s.rfind("new"));
    assert(sp.rfind("st") == s.rfind("st"));
    assert(sp.rfind("i", 22) == s.rfind("i", 22));
    assert(sp.rfind("i", 21) == s.rfind("i", 21));
    
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