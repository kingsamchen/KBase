/*
 @ 0xCCCCCCCC
*/

#include "kbase/string_piece.h"

#include <ostream>

namespace kbase {

namespace internal {

static inline void BuildLookupTable(const StringPiece& characters,
                                    std::bitset<UCHAR_MAX + 1>* table)
{
    for (size_t i = 0; i < characters.size(); ++i) {
        table->set(static_cast<unsigned char>(characters[i]));
    }
}

StringPiece::size_type find_first_of(const StringPiece& self,
                                     const StringPiece& s,
                                     StringPiece::size_type pos)
{
    if (self.empty() || s.empty())
        return StringPiece::npos;

    // avoid creating a look-up table in this case
    if (s.size() == 1)
        return find(self, s, pos);

    std::bitset<UCHAR_MAX + 1> lookup_table;
    BuildLookupTable(s, &lookup_table);
    for (StringPiece::size_type i = pos; i < self.size(); ++i) {
        if (lookup_table.test(static_cast<unsigned char>(self[i]))) {
            return i;
        }
    }

    return StringPiece::npos;
}

static StringPiece::size_type find_first_not_of(const StringPiece& self,
                                                char ch,
                                                StringPiece::size_type pos)
{
    for (StringPiece::size_type i = pos; i < self.size(); ++i) {
        if (self[i] != ch)
            return i;
    }

    return StringPiece::npos;
}

StringPiece::size_type find_first_not_of(const StringPiece& self,
                                         const StringPiece& s,
                                         StringPiece::size_type pos)
{
    if (self.empty() || s.empty())
        return StringPiece::npos;

    if (s.size() == 1)
        return find_first_not_of(self, s[0], pos);

    std::bitset<UCHAR_MAX + 1> lookup_table;
    BuildLookupTable(s, &lookup_table);
    for (StringPiece::size_type i = pos; i < self.size(); ++i) {
        if (!lookup_table.test(static_cast<unsigned char>(self[i])))
            return i;
    }

    return StringPiece::npos;
}

StringPiece::size_type find_last_of(const StringPiece& self,
                                    const StringPiece& s,
                                    StringPiece::size_type pos)
{
    if (self.empty() || s.empty())
        return StringPiece::npos;

    if (s.size() == 1)
        return rfind(self, s, pos);

    std::bitset<UCHAR_MAX + 1> lookup_table;
    BuildLookupTable(s, &lookup_table);

    auto begin_pos = std::min(pos, self.size() - 1);
    for (StringPiece::size_type i = begin_pos; ; --i) {
        if (lookup_table.test(static_cast<unsigned char>(self[i])))
            return i;

        if (i == 0) break;
    }

    return StringPiece::npos;
}

static StringPiece::size_type find_last_not_of(const StringPiece& self,
                                               char ch,
                                               StringPiece::size_type pos)
{
    auto begin_pos = std::min(pos, self.size() - 1);
    for (StringPiece::size_type i = begin_pos; ; --i) {
        if (self[i] != ch)
            return i;

        if (i == 0) break;
    }

    return StringPiece::npos;
}

StringPiece::size_type find_last_not_of(const StringPiece& self,
                                        const StringPiece& s,
                                        StringPiece::size_type pos)
{
    if (self.empty() || s.empty())
        return StringPiece::npos;

    if (s.size() == 1)
        return find_last_not_of(self, s[0], pos);

    std::bitset<UCHAR_MAX + 1> lookup_table;
    BuildLookupTable(s, &lookup_table);

    auto begin_pos = std::min(pos, self.size() - 1);
    for (StringPiece::size_type i = begin_pos; ; --i) {
        if (!lookup_table.test(static_cast<unsigned char>(self[i])))
            return i;

        if (i == 0) break;
    }

    return StringPiece::npos;
}

}   // namespace internal

std::ostream& operator<<(std::ostream& os, const StringPiece& s)
{
    os.write(s.data(), static_cast<std::streamsize>(s.size()));

    return os;
}

std::wostream& operator<<(std::wostream& os, const WStringPiece& s)
{
    os.write(s.data(), static_cast<std::streamsize>(s.size()));

    return os;
}

}   // namespace kbase


