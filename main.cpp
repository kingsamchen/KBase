
#include <cassert>
#include <cstdio>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "kbase/files/file_path.h"

template<typename T>
void print_out(T beg, T end, const char* dem = " ")
{
    for (auto it = beg; it != end; ++it) {
        std::wcout << *it << dem;
    }

    std::wcout << std::endl;
}

int main(int /*argc*/, char* /*argv[]*/)
{
    std::wstring data[] = {
        L"",
        L"aa",
        L"/aa/bb",
        L"/aa/bb/",
        L"/aa/bb//",
        L"/aa/bb/ccc",
        L"/aa",
        L"/",
        L"//",
        L"///",
        L"aa/",
        L"aa/bb",
        L"aa/bb/",
        L"aa/bb//",
        L"aa//bb//",
        L"aa//bb/",
        L"aa//bb",
        L"//aa/bb",
        L"//aa/",
        L"//aa",
        L"0:",
        L"@:",
        L"[:",
        L"`:",
        L"{:"
    };

    for (const auto& s : data) {
        kbase::FilePath path(s);
        std::wcout << s << L"\t-->\t" << path.BaseName().value() << std::endl;
    }

    _getch();
    return 0;
}

#ifdef _DEBUG
namespace kbase {
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