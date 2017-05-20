/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_REGISTRY_H_
#define KBASE_REGISTRY_H_

#include <array>
#include <string>
#include <vector>

#include <Windows.h>

#include "kbase/basic_macros.h"

namespace kbase {

// A RegKey instance represents an opened registry key on Windows.
// Create() or Open() may result in an invalid RegKey instance, which will throw an
// exception if further operations involving registry manipulations are performed.
// Validity of an RegKey instance can be indicated by boolean test.

class RegKey {
public:
    enum WOW6432Node {
        DefaultKeyOnWOW64,
        Force32KeyOnWOW64,
        Force64KeyOnWOW64
    };

    // The created instance remains invalid until it succeeds in calling Open() to open
    // a registry key.
    RegKey() noexcept;

    RegKey(RegKey&& other) noexcept;

    RegKey& operator=(RegKey&& other) noexcept;

    ~RegKey();

    DISALLOW_COPY(RegKey);

    // Create or open a registry key, and return a RegKey instance to represent.
    // The `disposition` indicates the exact behavior.

    static RegKey Create(HKEY rootkey, const wchar_t* subkey, REGSAM access);

    static RegKey Create(HKEY rootkey, const wchar_t* subkey, REGSAM access, DWORD& disposition);

    // `subkey` here is relative to the current registry key.
    void Open(const wchar_t* subkey, REGSAM access);

    void Open(HKEY rootkey, const wchar_t* subkey, REGSAM access);

    HKEY Get() const noexcept;

    // Releases the ownership of the opened key.
    HKEY Release() noexcept;

    void Close() noexcept;

    explicit operator bool() const noexcept
    {
        return IsValid();
    }

    // Returns true if the registry key exists; Returns false if it doesn't.
    // For details about WOW6432Node redirection, see @ http://is.gd/6Z23qk
    static bool KeyExists(HKEY rootkey, const wchar_t* subkey,
                          WOW6432Node node_key = WOW6432Node::DefaultKeyOnWOW64);

    // Removes a specific key along with all subkeys it contains. Use with care.
    // The key must have been opened with DELETE, KEY_ENUMERATE_SUB_KEYS, KEY_QUERY_VALUE,
    // and KEY_SET_VALUE access rights.
    // `key_name` may be nullptr, if you want to delete all subkeys of the current
    // registry key.
    void DeleteKey(const wchar_t* key_name) const;

    // Returns true, if this key has the specified value.
    // Returns false if it doesn't.
    bool HasValue(const wchar_t* value_name) const;

    // Gets count of values in key.
    size_t GetValueCount() const;

    // `index` is in the range [0, value_count).
    void GetValueNameAt(size_t index, std::wstring& value_name) const;

    // Removes a specific value under the key.
    void DeleteValue(const wchar_t* value_name) const;

    // Reads a REG_DWORD registry field.
    void ReadValue(const wchar_t* value_name, DWORD& value) const;

    // Reads a REG_QWORD registry field.
    void ReadValue(const wchar_t* value_name, DWORD64& value) const;

    // Reads a REG_SZ or a REG_EXPAND_SZ registry field into the `value`.
    void ReadValue(const wchar_t* value_name, std::wstring& value) const;

    // Reads a REG_MULTI_SZ registry field into a vector of strings.
    void ReadValue(const wchar_t* value_name, std::vector<std::wstring>& values) const;

    // Reads raw data, and `data_type` indicates the what the type of the data is.

    void ReadRawValue(const wchar_t* value_name, DWORD restricted_type, void* data,
                      DWORD& data_size) const;

    void ReadRawValue(const wchar_t* value_name, void* data, DWORD& data_size,
                      DWORD& data_type) const;

    // Sets a REG_DWORD registry field.
    void WriteValue(const wchar_t* value_name, DWORD value) const;

    // Sets a REG_QWORD registry field.
    void WriteValue(const wchar_t* value_name, DWORD64 value) const;

    // Sets a REG_SZ registry field.
    void WriteValue(const wchar_t* value_name, const wchar_t* value, size_t length) const;

    // Sets a raw data into a specific registry field.
    void WriteValue(const wchar_t* value_name, const void* data, size_t data_size,
                    DWORD data_type) const;

private:
    RegKey(HKEY rootkey, const wchar_t* subkey, REGSAM access, DWORD& disposition);

    bool IsValid() const noexcept
    {
        return key_ != nullptr;
    }

private:
    HKEY key_;
};

class RegKeyIterator {
public:
    // If construction fails, the internal state is invalid.
    RegKeyIterator(HKEY rootkey, const wchar_t* folder_key);

    ~RegKeyIterator();

    // Support for move-semantics
    RegKeyIterator(RegKeyIterator&& other);
    RegKeyIterator& operator=(RegKeyIterator&& other);

    DISALLOW_COPY(RegKeyIterator);

    bool Valid() const
    {
        return key_ != nullptr && index_ >= 0;
    }

    int index() const
    {
        return index_;
    }

    size_t subkey_count() const
    {
        return subkey_count_;
    }

    const wchar_t* key_name() const
    {
        return key_name_.data();
    }

    RegKeyIterator& operator++();

private:
    bool Read();
    void Close();

private:
    HKEY key_;
    int index_; // when index_ becomes negative, enumeration is done.
    size_t subkey_count_;
    std::array<wchar_t, MAX_PATH> key_name_;
};

class RegValueIterator {
private:
    enum {
        INITIAL_NAME_SIZE = MAX_PATH,
        INITIAL_VALUE_SIZE = 512
    };

public:
    RegValueIterator(HKEY rootkey, const wchar_t* folder_key);

    ~RegValueIterator();

    RegValueIterator(const RegValueIterator&) = delete;
    RegValueIterator& operator=(const RegValueIterator&) = delete;

    // Support for move-semantics.
    RegValueIterator(RegValueIterator&& other);
    RegValueIterator& operator=(RegValueIterator&& other);

    bool Valid() const
    {
        return key_ != nullptr && index_ >= 0;
    }

    int index() const
    {
        return index_;
    }

    size_t value_count() const
    {
        return value_count_;
    }

    DWORD type() const
    {
        return type_;
    }

    const wchar_t* value_name() const
    {
        return value_name_.c_str();
    }

    size_t value_size() const
    {
        return value_size_;
    }

    const char* value() const
    {
        return value_.data();
    }

    RegValueIterator& operator++();

private:
    bool Read();
    void Close();

private:
    HKEY key_;
    int index_;
    size_t value_count_;
    DWORD max_value_name_length_;  // always in characters.
    DWORD max_value_length_;       // in bytes.
    DWORD type_;
    std::wstring value_name_;
    std::vector<char> value_;
    size_t value_size_;
};

}   // namespace kbase

#endif  // KBASE_REGISTRY_H_
