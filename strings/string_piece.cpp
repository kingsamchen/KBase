// Author:  Kingsley Chen
// Date:    2014/01/23
// Purpose: implementation for specialized find_*_of operations
//          for std::string         

#include "string_piece.h"

namespace KBase {

namespace internal {

static inline void BuildLookupTable(const StringPiece& characters,
                                    std::bitset<UCHAR_MAX + 1>* table)
{
    for (size_t i = 0; i < characters.size(); i++) {
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

    for (StringPiece::size_type i = pos; i < self.size(); i++) {
        if (lookup_table.test(static_cast<unsigned char>(self[i]))) {
            return i;
        }
    }

    return StringPiece::npos;
}

}

}


