// Author:  Kingsley Chen
// Date:    2014/01/23
// Purpose: core implementation of BasicStringPiece
//          and its underlying StringPieceDetail

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_STRINGS_STRING_PIECE_H_
#define KBASE_STRINGS_STRING_PIECE_H_

// try to include few header files as you could to speed up compilation

#include <iosfwd>
#include <algorithm>
#include <bitset>
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
        size_type min_length = std::min(length_, other.length_);
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

// internal helper functions

template<typename STRING_TYPE>
void CopyToString(const BasicStringPiece<STRING_TYPE>& self, STRING_TYPE* target)
{
    target->assign(self.cbegin(), self.cend());
}

template<typename STRING_TYPE>
void AppendToString(const BasicStringPiece<STRING_TYPE>& self, STRING_TYPE* target)
{
    target->append(self.cbegin(), self.cend());
}

template<typename STRING_TYPE>
typename BasicStringPiece<STRING_TYPE>::size_type
    copy(const BasicStringPiece<STRING_TYPE>& self,
         typename BasicStringPiece<STRING_TYPE>::value_type* data,
         typename BasicStringPiece<STRING_TYPE>::size_type n,
         typename BasicStringPiece<STRING_TYPE>::size_type pos)
{
    typename BasicStringPiece<STRING_TYPE>::size_type remain
        = std::min(self.size() - pos, n);
    memcpy_s(data, n, self.data() + pos, remain);

    return remain;
}

template<typename STRING_TYPE>
typename BasicStringPiece<STRING_TYPE>::size_type
    find(const BasicStringPiece<STRING_TYPE>& self,
         const BasicStringPiece<STRING_TYPE>& s,
         typename BasicStringPiece<STRING_TYPE>::size_type pos)
{
    if (self.size() < s.size() + pos)
        return BasicStringPiece<STRING_TYPE>::npos;

    // if no matched range found, |std::search| returns self.cend()
    // but a valid return value must be within [begin, end - s.length + 1)
    auto result_it = std::search(self.cbegin() + pos, self.cend(), s.cbegin(), s.cend());
    typename BasicStringPiece<STRING_TYPE>::size_type xpos =
        static_cast<size_t>(result_it - self.cbegin());

    return (xpos + s.size() <= self.size()) ?
                xpos : BasicStringPiece<STRING_TYPE>::npos;
}

template<typename STRING_TYPE>
typename BasicStringPiece<STRING_TYPE>::size_type
    find(const BasicStringPiece<STRING_TYPE>& self,
         typename BasicStringPiece<STRING_TYPE>::value_type ch,
         typename BasicStringPiece<STRING_TYPE>::size_type pos)
{
    if (self.size() <= pos)
        return BasicStringPiece<STRING_TYPE>::npos;

    auto result_it = std::find(self.cbegin() + pos, self.cend(), ch);
    
    return result_it != self.cend() ? 
        static_cast<size_t>(result_it - self.cbegin()) :
        BasicStringPiece<STRING_TYPE>::npos;
}

template<typename STRING_TYPE>
typename BasicStringPiece<STRING_TYPE>::size_type
    rfind(const BasicStringPiece<STRING_TYPE>& self,
          const BasicStringPiece<STRING_TYPE>& s,
          typename BasicStringPiece<STRING_TYPE>::size_type pos)
{
    if (self.size() < s.size())
        return BasicStringPiece<STRING_TYPE>::npos;

    // refer to http://en.cppreference.com/w/cpp/string/basic_string/rfind
    if (s.empty())
        return std::min(pos, s.size() - 1);

    auto last = self.cbegin() + std::min(self.size() - s.size(), pos) + s.size();
    auto result_it = std::find_end(self.cbegin(), last, s.cbegin(), s.cend());

    return result_it != last ? 
        static_cast<size_t>(result_it - self.cbegin()) : 
        BasicStringPiece<STRING_TYPE>::npos;
}

template<typename STRING_TYPE>
typename BasicStringPiece<STRING_TYPE>::size_type
    rfind(const BasicStringPiece<STRING_TYPE>& self,
          typename BasicStringPiece<STRING_TYPE>::value_type ch,
          typename BasicStringPiece<STRING_TYPE>::size_type pos)
{
    typename BasicStringPiece<STRING_TYPE>::size_type begin_pos =
        std::min(pos, self.size() - 1);

    auto result_it = 
        std::find(self.crbegin() + ((self.size() - 1) - begin_pos), self.crend(), ch);
    
    return result_it != self.crend() ?
        static_cast<size_t>(result_it.base() - 1 - self.cbegin()) :
        BasicStringPiece<STRING_TYPE>::npos;
}

template<typename STRING_TYPE>
typename BasicStringPiece<STRING_TYPE>::size_type
    find_first_of(const BasicStringPiece<STRING_TYPE>& self,
                  const BasicStringPiece<STRING_TYPE>& s,
                  typename BasicStringPiece<STRING_TYPE>::size_type pos)
{
    if (self.empty() || s.empty())
        return BasicStringPiece<STRING_TYPE>::npos;

    for (BasicStringPiece<STRING_TYPE>::size_type i = pos; i < self.size(); ++i) {
        for (BasicStringPiece<STRING_TYPE>::size_type j = 0; j < s.size(); ++j) {
            if (self[i] == s[j])
                return i;
        }
    }

    return BasicStringPiece<STRING_TYPE>::npos;
}

template<typename STRING_TYPE>
typename BasicStringPiece<STRING_TYPE>::size_type
    find_first_not_of(const BasicStringPiece<STRING_TYPE>& self,
                      const BasicStringPiece<STRING_TYPE>& s,
                      typename BasicStringPiece<STRING_TYPE>::size_type pos)
{
    if (self.empty() || s.empty())
        return BasicStringPiece<STRING_TYPE>::npos;

    for (BasicStringPiece<STRING_TYPE>::size_type i = pos; i < self.size(); ++i) {
        for (BasicStringPiece<STRING_TYPE>::size_type j = 0; j < s.size(); ++j) {
            if (self[i] != s[j])
                return i;
        }
    }

    return BasicStringPiece<STRING_TYPE>::npos;
}

// for find_last_*_of functions, only substring in the range [0, pos] will
// be examined

template<typename STRING_TYPE>
typename BasicStringPiece<STRING_TYPE>::size_type
    find_last_of(const BasicStringPiece<STRING_TYPE>& self,
                 const BasicStringPiece<STRING_TYPE>& s,
                 typename BasicStringPiece<STRING_TYPE>::size_type pos)
{
    if (self.empty() || s.empty())
        return BasicStringPiece<STRING_TYPE>::npos;

    auto begin_pos = std::min(pos, self.size() - 1);
    for (BasicStringPiece<STRING_TYPE>::size_type i = begin_pos; ; --i) {
        for (BasicStringPiece<STRING_TYPE>::size_type j = 0; j < s.size(); ++j) {
            if (self[i] == s[j])
                return i;
        }

        if (i == 0) break;
    }
    
    return BasicStringPiece<STRING_TPYE>::npos;
}

template<typename STRING_TYPE>
typename BasicStringPiece<STRING_TYPE>::size_type
    find_last_not_of(const BasicStringPiece<STRING_TYPE>& self,
                     const BasicStringPiece<STRING_TYPE>& s,
                     typename BasicStringPiece<STRING_TYPE>::size_type pos)
{
    if (self.empty() || s.empty())
        return BasicStringPiece<STRING_TYPE>::npos;

    auto begin_pos = std::min(pos, self.size() - 1);
    for (BasicStringPiece<STRING_TYPE>::size_type i = begin_pos; ; --i) {
        for (BasicStringPiece<STRING_TYPE>::size_type j = 0; j < s.size(); ++j) {
            if (self[i] != s[j])
                return i;
        }

        if (i == 0) break;
    }

    return BasicStringPiece<STRING_TYPE>::npos;
}

template<typename STRING_TYPE>
BasicStringPiece<STRING_TYPE> substr(const BasicStringPiece<STRING_TYPE>& self,
                                     typename BasicStringPiece<STRING_TYPE>::size_type pos,
                                     typename BasicStringPiece<STRING_TYPE>::size_type n)
{
    auto begin_pos = std::min(pos, self.size());
    if (begin_pos + n > self.size())
        n = self.size() - begin_pos;
        
    return BasicStringPiece<STRING_TYPE>(self.data() + begin_pos, n);
}

// specialized internal find_*_of operations for std::string

StringPieceDetail<std::string>::size_type 
    find_first_of(const StringPiece& self,
                  const StringPiece& s,
                  StringPieceDetail<std::string>::size_type pos = 0);

StringPieceDetail<std::string>::size_type 
    find_first_not_of(const StringPiece& self,
                      const StringPiece& s,
                      StringPieceDetail<std::string>::size_type pos = 0);

StringPieceDetail<std::string>::size_type
    find_last_of(const StringPiece& self,
                 const StringPiece& s,
                 StringPieceDetail<std::string>::size_type pos = 0);

StringPieceDetail<std::string>::size_type
    find_last_not_of(const StringPiece& self,
                     const StringPiece& s,
                     StringPieceDetail<std::string>::size_type pos = 0);

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

    void CopyToString(STRING_TYPE* target) const
    {
        internal::CopyToString(*this, target);
    }

    void AppendToString(STRING_TYPE* target) const
    {
        internal::AppendToString(*this, target);
    }

    size_type copy(value_type* data, size_type n, size_type pos = 0) const
    {
        return internal::copy(*this, data, n, pos);
    }

    bool StartsWith(const BasicStringPiece& token) const
    {
        return ((length_ >= token.length_) &&
                (wordmemcmp(ptr_, token.ptr_, token.length_) == 0));
    }

    bool EndsWith(const BasicStringPiece& token) const
    {
        return ((length_ >= token.length_) &&
                (wordmemcmp(ptr_ + length_ - token.length_, token.ptr_, token.length_) == 0));
    }

    size_type find(const BasicStringPiece& s, size_type pos = 0) const
    {
        return internal::find(*this, s, pos);
    }

    size_type find(value_type ch, size_type pos = 0) const
    {
        return internal::find(*this, ch, pos);
    }

    // |rfind| functions find the last substring equal to the given character
    // sequence within the range [begin, pos + s.size]. 
    // therefor, substring that begins at which following the pos is passed

    size_type rfind(const BasicStringPiece& s, size_type pos = npos) const
    {
        return internal::rfind(*this, s, pos);
    }

    size_type rfind(value_type ch, size_type pos = npos) const
    {
        return internal::rfind(*this, ch, pos);
    }

    size_type find_first_of(const BasicStringPiece& s, size_type pos = 0) const
    {
        return internal::find_first_of(*this, s, pos);
    }

    size_type find_first_not_of(const BasicStringPiece& s, size_type pos = 0) const
    {
        return internal::find_first_not_of(*this, s, pos);
    }

    size_type find_last_of(const BasicStringPiece& s, size_type pos = npos) const
    {
        return internal::find_last_of(*this, s, pos);
    }

    size_type find_last_not_of(const BasicStringPiece& s, size_type pos = npos) const
    {
        return internal::find_last_not_of(*this, s, pos);
    }

    BasicStringPiece substr(size_type pos, size_type n = npos) const
    {
        return internal::substr(*this, pos, n);
    }
};

// specialized find_*_of operations for std::string

template<>
StringPiece::size_type
    BasicStringPiece<std::string>::find_first_of(const StringPiece& s, size_type pos) const
{
    return internal::find_first_of(*this, s, pos);
}

template<>
StringPiece::size_type
    BasicStringPiece<std::string>::find_first_not_of(const StringPiece& s, size_type pos) const
{
    return internal::find_first_not_of(*this, s, pos);
}

template<>
StringPiece::size_type
    BasicStringPiece<std::string>::find_last_of(const StringPiece& s, size_type pos) const
{
    return internal::find_last_of(*this, s, pos);
}

template<>
StringPiece::size_type
    BasicStringPiece<std::string>::find_last_not_of(const StringPiece& s, size_type pos) const
{
    return internal::find_last_not_of(*this, s, pos);
}

}   // namespace KBase

#endif  // KBASE_STRINGS_STRING_PIECE_H_