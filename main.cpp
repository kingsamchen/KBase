
#include <cassert>
#include <cstdio>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "kbase/files/file_enumerator.h"

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
    kbase::FilePath root(L"d:\\MyDownload");
    kbase::FileEnumerator file_enumer(root, false, kbase::FileEnumerator::FILES);
    for (kbase::FilePath file = file_enumer.Next(); !file.empty();
         file = file_enumer.Next()) {
        std::wcout << file.value() << L"\t";
        std::wcout << file_enumer.GetInfo().GetSize() << std::endl;
    }

    _getch();
    return 0;
}
