// KBase_Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

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
#include "kbase/version_util.h"

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
    kbase::StringPiece sp("hello world");
    std::cout << sp;

    _getch();
    return 0;
}
