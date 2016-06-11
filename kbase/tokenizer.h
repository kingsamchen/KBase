/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_TOKENIZER_H_
#define KBASE_TOKENIZER_H_

#include "kbase/error_exception_util.h"
#include "kbase/string_view.h"

namespace kbase {

// Both `BasicTokenizer` and its `iterator` access source data via `BasicStringView` objects.
// That is, they just visit the source.

template<typename CharT>
class TokenIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using token_type = BasicStringView<CharT>;
    using value_type = token_type;
    using difference_type = ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;

    TokenIterator(token_type data, size_t offset, token_type delim)
        : data_(data), delim_(delim), offset_(offset)
    {
        GetToken();
    }

    TokenIterator(const TokenIterator&) noexcept = default;

    TokenIterator(TokenIterator&&) noexcept = default;

    TokenIterator& operator=(const TokenIterator&) noexcept = default;

    TokenIterator& operator=(TokenIterator&&) noexcept = default;

    ~TokenIterator() = default;

    TokenIterator& operator++()
    {
        ENSURE(CHECK, offset_ != data_.length()).Require("iterator now is not incrementable");
        offset_ = next_offset_;
        GetToken();
        return *this;
    }

    TokenIterator operator++(int)
    {
        TokenIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    reference operator*() const
    {
        return current_token_;
    }

    pointer operator->() const
    {
        return &current_token_;
    }

    friend bool operator==(const TokenIterator& lhs, const TokenIterator& rhs)
    {
        return lhs.data_.data() == rhs.data_.data() &&
            lhs.data_.length() == rhs.data_.length() &&
            lhs.offset_ == rhs.offset_;
    }

    friend bool operator!=(const TokenIterator& lhs, const TokenIterator& rhs)
    {
        return !(lhs == rhs);
    }

private:
    void GetToken()
    {
        if (offset_ == data_.length()) {
            current_token_ = token_type();
            next_offset_ = data_.length();
            return;
        }

        size_t token_begin = data_.find_first_not_of(delim_, offset_);
        if (token_begin == token_type::npos) {
            current_token_ = token_type();
            next_offset_ = data_.length();
            return;
        }

        size_t token_end = data_.find_first_of(delim_, token_begin);
        if (token_end == token_type::npos) {
            token_end = data_.length();
        }

        current_token_ = token_type(data_.data() + token_begin, token_end - token_begin);
        next_offset_ = token_end;
    }

private:
    token_type data_;
    token_type delim_;
    size_t offset_;
    token_type current_token_;
    size_t next_offset_ = 0ULL;
};

template<typename CharT>
class BasicTokenizer {
public:
    using token_type = BasicStringView<CharT>;
    using iterator = TokenIterator<CharT>;
    using const_iterator = iterator;

    explicit constexpr BasicTokenizer(BasicStringView<CharT> str, BasicStringView<CharT> delim)
        : data_(str), delim_(delim)
    {}

    BasicTokenizer(const BasicTokenizer&) noexcept = default;

    BasicTokenizer(BasicTokenizer&&) noexcept = default;

    BasicTokenizer& operator=(const BasicTokenizer&) noexcept = default;

    BasicTokenizer& operator=(BasicTokenizer&&) noexcept = default;

    ~BasicTokenizer() = default;

    iterator begin() const
    {
        return iterator(data_, 0, delim_);
    }

    iterator end() const
    {
        return iterator(data_, data_.length(), delim_);
    }

private:
    token_type data_;
    token_type delim_;
};

using Tokenizer = BasicTokenizer<char>;
using WTokenizer = BasicTokenizer<wchar_t>;

}   // namespace kbase

#endif  // KBASE_TOKENIZER_H_