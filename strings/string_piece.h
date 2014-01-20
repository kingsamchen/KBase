// Author:  Kingsley Chen
// Date:    2014/01/20
// Purpose: core implementation of BasicStringPiece
//          and its underlying StringPieceDetail

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_STRINGS_STRING_PIECE_H_
#define KBASE_STRINGS_STRING_PIECE_H_

// try to include few header files as you could to speed up compilation

#include <iosfwd>
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
    StringPieceDetail() : ptr_(nullptr), length_(0)
    {}

    StringPieceDetail(const value_type* str) 
        : ptr_(str), length_(typename STRING_TYPE::traits_type::length(str)) 
    {}

    StringPieceDetail(const STRING_TYPE& str) 
        : ptr_(str.data()), length_(str.length())
    {}

    StringPieceDetail(const value_type* data, size_type len) 
        : ptr_(data), length_(len)
    {}

    StringPieceDetail(const typename STRING_TYPE::const_iterator& cbegin,
                      const typename STRING_TYPE::const_iterator& cend)
        : ptr_(cbegin < cend ? &(*cbegin) : nullptr),
          length_(cbegin < cend ? static_cast<size_type>(cend - cbegin) : 0)
    {}

    
    const value_type* data() const
    {
        return ptr_;
    }

    size_type length() const
    {
        return length_;
    }

    size_type size() const
    {
        return length_;
    }

    size_type capacity() const
    {
        // since we cannot change the memory the internal string resides,
        // we simply treat its capacity same with the length.
        return size();
    }

    bool empty() const
    {
        return length_ == 0;
    }

    void clear()
    {
        ptr_ = nullptr;
        length_ = 0;
    }

    void set(const value_type* str)
    {
        ptr_ = str;
        length_ = STRING_TYPE::traits_type::length(str);
    }

    void set(const value_type* data, size_type len)
    {
        ptr_ = data;
        length_ = len;
    }

    value_type operator[](size_type i) const
    {
        return ptr_[i];
    }

    int compare(const BasicStringPiece<STRING_TYPE>& other) const
    {
        size_type min_length = length_ < other.length_ ? length_ : other.length_;
        int ret = wordmemcmp(ptr_, other.ptr_, min_length);
        
        if (ret == 0) {
            if (length_ < other.length_) {
                return -1;
            } else {
                return 1;
            }
        }

        return ret;
    }
    
    void RemovePrefix(size_type n)
    {
        ptr_ += n;
        length_ -= n;
    }

    void RemoveSuffix(size_type n)
    {
        length_ -= n;
    }

    static int wordmemcmp(const value_type* str1, const value_type* str2,
                          size_type n)
    {
        return STRING_TYPE::traits_type::compare(str1, str2, n);
    }

    STRING_TYPE as_string() const
    {
        return empty() ? STRING_TYPE() : STRING_TYPE(ptr_, length_);
    }

    const_iterator cbegin() const
    {
        return ptr_;
    }

    const_iterator cend() const
    {
        return ptr_ + length_;
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(ptr_ + length_);
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(ptr_);
    }

protected:
    const value_type* ptr_;
    size_type length_;
};

template<typename STRING_TYPE>
const typename StringPieceDetail<STRING_TYPE>::size_type
StringPieceDetail<STRING_TYPE>::npos = 
    static_cast<typename StringPieceDetail<STRING_TYPE>::size_type>(-1);

}   // namespace internal

template<typename STRING_TYPE>
class BasicStringPiece : public internal::StringPieceDetail<STRING_TYPE> {
public:
    typedef typename internal::StringPieceDetail<STRING_TYPE>::value_type 
        value_type;
    typedef typename internal::StringPieceDetail<STRING_TYPE>::size_type
        size_type;

public:
    BasicStringPiece()
    {}

    BasicStringPiece(const value_type* str)
        : internal::StringPieceDetail<STRING_TYPE>(str)
    {}

    BasicStringPiece(const value_type* data, size_type len)
        : internal::StringPieceDetail<STRING_TYPE>(data, len)
    {}

    BasicStringPiece(const STRING_TYPE& str)
        : internal::StringPieceDetail<STRING_TYPE>(str)
    {}

    BasicStringPiece(const typename STRING_TYPE::const_iterator& cbegin,
                     const typename STRING_TYPE::const_iterator& cend)
        : internal::StringPieceDetail<STRING_TYPE>(cbegin, cend)
    {}

};

}   // namespace KBase

#endif  // KBASE_STRINGS_STRING_PIECE_H_

