// Author:  Kingsley Chen
// Date:    2014/01/20
// Purpose: core implementation of BasicStringPiece
//          and its underlying StringPieceDetail

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_STRINGS_STRING_PIECE_H_
#define KBASE_STRINGS_STRING_PIECE_H_

#include <string>

namespace KBase {

template<typename STRING_TYPE> class BasicStringPiece;
typedef BasicStringPiece<std::string> StringPiece;
typedef BasicStringPiece<std::wstring> WStringPiece;

namespace internal {

// define the types, methods, operations and data common to both StringPiece
// and WStringPiece.
template<typename STRING_TYPE>
class StringPieceDetail {
public:
    typedef size_t size_type;
    typedef typename STRING_TYPE::value_type value_type;
    typedef const value_type* const_pointer;
    typedef const value_type& const_reference;
    typedef ptrdiff_t difference_type;
    typedef const value_type* const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    static const size_type npos;

public:
    // TODO: complete ctors
    StringPieceDetail();
    StringPieceDetail(const value_type* str);
    StringPieceDetail(const STRING_TYPE& str);
    StringPieceDetail(const value_type* ptr, size_type len);
    StringPieceDetail(const typename STRING_TYPE::const_iterator& begin,
                      const typename STRING_TYPE::const_iterator& end);


protected:
    cosnt value_type* ptr_;
    size_type length_;
};

}   // namespace internal

}   // namespace KBase

#endif  // KBASE_STRINGS_STRING_PIECE_H_

