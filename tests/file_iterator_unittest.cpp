/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include <fstream>
#include <iostream>

#include "kbase/file_iterator.h"

#if defined(OS_WIN)
#include "kbase/string_encoding_conversions.h"
#endif

namespace kbase {

TEST_CASE("List files on a path", "[FileIterator]")
{
    int count = 0;
    for (FileIterator fit(Path(PATH_LITERAL(".")), false); fit != FileIterator(); ++fit) {
        ++count;
    }

    REQUIRE(count > 0);

    std::cout << "Found "<< count << " files in current directory.\n";
}

TEST_CASE("Use range-based for syntax", "[FileIterator]")
{
    int count = 0;
    for (const auto& entry : FileIterator(Path(PATH_LITERAL(".")), false)) {
        UNUSED_VAR(entry);
        ++count;
    }

    REQUIRE(count > 0);
}

TEST_CASE("List files on a path and its sub-directories", "[FileIterator]")
{
    int count = 0;
    for (const auto& entry : FileIterator(Path(PATH_LITERAL(".")), true)) {
        UNUSED_VAR(entry);
        ++count;
    }

    REQUIRE(count > 0);

    std::cout << "Found "<< count << " files (including sub-files) in current directory.\n";
}

TEST_CASE("Iterator should be copyable", "[FileIterator]")
{
    {
        std::ofstream f1("file_iterator_test1");
        std::ofstream f2("file_iterator_test2");
        std::ofstream f3("file_iterator_test3");
    }

    FileIterator it(Path(PATH_LITERAL(".")), false);
    auto first_entry = it->file_path();
#if defined(OS_WIN)
    std::cout << "first entry: " << WideToASCII(first_entry.value()) << std::endl;
#else
    std::cout << "first entry: " << first_entry.value() << std::endl;
#endif

    // `cit` and `it` should both point to the same entry.
    auto cit = it;
    REQUIRE((cit == it));
    REQUIRE(cit->file_path().value() == it->file_path().value());
    REQUIRE(cit->file_path().value() == first_entry.value());

    // After increment of cit, `it` is invalidated and is even no longer required to be
    // dereferenceable.
    ++cit;
#if defined(OS_WIN)
    std::cout << "++cit entry: " << WideToASCII(cit->file_path().value()) << std::endl;
#else
    std::cout << "++cit entry: " << cit->file_path().value() << std::endl;
#endif
    REQUIRE(cit->file_path().value() != first_entry.value());

    it = cit;
    ++it;
#if defined(OS_WIN)
    std::cout << "++it entry: " << WideToASCII(it->file_path().value()) << std::endl;
#else
    std::cout << "++it entry: " << it->file_path().value() << std::endl;
#endif
    REQUIRE(it->file_path() != first_entry);
}

}   // namespace kbase
