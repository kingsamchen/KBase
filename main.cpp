
#include <cassert>
#include <cstdio>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "strings/sys_string_encoding_conversions.h"

using std::cout;
using std::endl;

int main(int /*argc*/, char* /*argv[]*/)
{
    std::ofstream out("encoding_test.txt");
    std::string s = "helloÖÐÎÄ";
    std::wstring ws = KBase::SysMultiByteToWide(s, KBase::CodePage::Default_Code_Page);
    std::string utf8s = KBase::SysWideToMultiByte(ws, KBase::CodePage::UTF8);
    out << utf8s;
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