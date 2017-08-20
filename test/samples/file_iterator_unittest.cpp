/*
 @ 0xCCCCCCCC
*/

#include "gtest/gtest.h"

#include "kbase/file_iterator.h"

#if defined(OS_WIN)
#include "kbase/string_encoding_conversions.h"
#endif

namespace kbase {

TEST(FileIteratorTest, NonRecursiveIteration)
{
    int count = 0;
    for (FileIterator fit(Path(PATH_LITERAL(".")), false); fit != FileIterator(); ++fit) {
        ++count;
    }

    EXPECT_TRUE(count > 0);

    std::cout << "Found "<< count << " files in current directory.\n";
}

TEST(FileIteratorTest, RangeBasedIteration)
{
    int count = 0;
    for (const auto& entry : FileIterator(Path(PATH_LITERAL(".")), false)) {
        UNUSED_VAR(entry);
        ++count;
    }

    EXPECT_TRUE(count > 0);
}

TEST(FileIteratorTest, RecursiveIteration)
{
    int count = 0;
    for (const auto& entry : FileIterator(Path(PATH_LITERAL(".")), true)) {
        UNUSED_VAR(entry);
        ++count;
    }

    EXPECT_TRUE(count > 0);

    std::cout << "Found "<< count << " files (including sub-files) in current directory.\n";
}

TEST(FileIteratorTest, CopyIterator)
{
    FileIterator it(Path(PATH_LITERAL(".")), false);
    auto first_entry = it->file_path();
#if defined(OS_WIN)
    std::cout << "first entry: " << WideToASCII(first_entry.value()) << std::endl;
#else
    std::cout << "first entry: " << first_entry.value() << std::endl;
#endif

    auto cit = it;
    EXPECT_TRUE(cit == it);
    EXPECT_TRUE(cit->file_path() == it->file_path());
    EXPECT_TRUE(cit->file_path() == first_entry);

    ++cit;
#if defined(OS_WIN)
    std::cout << "++cit entry: " << WideToASCII(cit->file_path().value()) << std::endl;
#else
    std::cout << "++cit entry: " << cit->file_path().value() << std::endl;
#endif
    EXPECT_TRUE(it->file_path() != cit->file_path());
    EXPECT_TRUE(it->file_path() == first_entry);
    EXPECT_TRUE(it == cit);

    ++it;
#if defined(OS_WIN)
    std::cout << "++it entry: " << WideToASCII(it->file_path().value()) << std::endl;
#else
    std::cout << "++it entry: " << it->file_path().value() << std::endl;
#endif
    EXPECT_TRUE(it->file_path() != first_entry);
    EXPECT_TRUE(it->file_path() != cit->file_path());
}

}   // namespace kbase
