
#include <cassert>
#include <cstdio>
#include <conio.h>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "strings/string_util.h"

using std::cout;
using std::endl;

int main(int /*argc*/, char* /*argv[]*/)
{
    std::string str;
    std::vector<std::string> parts = {"a", "b", "c", "d"};
    std::cout << KBase::StringUtil::JoinString(parts, " -|- ");
    
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