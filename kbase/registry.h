/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_REGISTRY_H_
#define KBASE_REGISTRY_H_

#include <string>
#include <vector>

#include <Windows.h>

namespace kbase {

// Some operations when failed would throw Win32Expcetion.
class RegKey {
public:
    RegKey();
    
    explicit RegKey(HKEY key);
    
    // Creates or opens the specific registry key when constructing the object.
    // Throws an exception when fails.
    RegKey(HKEY rootkey, const wchar_t* subkey, REGSAM access);
    
    ~RegKey();

    RegKey(const RegKey&) = delete;

    RegKey& operator=(const RegKey&) = delete;

    // Support for move-semantics.
    RegKey(RegKey&& other);
    RegKey& operator=(RegKey&& other);

    // Release the ownership of the opened key.
    HKEY Release();

    // Creates or opens the registry key.
    // The |disposition| indicates exact behavior.
    // These functions throw an exception when it fails.
    void Create(HKEY rootkey, const wchar_t* subkey, REGSAM access);
    void Create(HKEY rootkey, const wchar_t* subkey, REGSAM access,
                DWORD* disposition);

    // Creates a subkey or opens it if it already exists.
    // Throws an exception when it fails.
    void CreateKey(const wchar_t* key_name, REGSAM access);

    // Opens an existing registry key.
    // Throws an exception when it fails.
    void Open(HKEY rootkey, const wchar_t* subkey, REGSAM access);

    // Opens an existing registry key with a given relative subkey name.
    // Throws an exception when it fails.
    void OpenKey(const wchar_t* key_name, REGSAM access);

    void Close();

    // Returns true, if this key has the specified value.
    // Returns false if it doesn't.
    // Throws an exception if an error occurs while attempting to access it.
    bool HasValue(const wchar_t* value_name) const;

    // Gets count of values in key.
    // Throws an exception when it fails.
    size_t GetValueCount() const;

    // |index| is in the range [0, value_count).
    // Throws an exception if an error occurs.
    void GetValueNameAt(size_t index, std::wstring* value_name) const;

    // Returns true, if the |key_| is valid.
    bool Valid() const
    {
        return key_ != nullptr;
    }

    // Removes a specific key along with all subkeys it contains. Use with care.
    // The key must have been opened with DELETE, KEY_ENUMERATE_SUB_KEYS, and
    // KEY_QUERY_VALUE access rights.
    // Throws an exception if an error occurs.
    void DeleteKey(const wchar_t* key_name);

    // Removes a specific value under the key.
    // Throws an exception if an error occurs.
    void DeleteValue(const wchar_t* value_name);

    // For each of the following read functions, it returns true if a function 
    // succeeds; and returns false, if an error occurs, and last error implies the 
    // reason for the failure.
    // Moreover, the variable that stores the value being read remains unchanged if
    // the function fails.

    // Reads a REG_DWORD registry field.
    bool ReadValue(const wchar_t* value_name, DWORD* value) const;

    // Reads a REG_QWORD registry field.
    bool ReadValue(const wchar_t* value_name, DWORD64* value) const;

    // Reads a REG_SZ or a REG_EXPAND_SZ registry field into the |value|.
    bool ReadValue(const wchar_t* value_name, std::wstring* value) const;

    // Reads a REG_MULTI_SZ registry field into a vector of strings.
    bool ReadValue(const wchar_t* value_name,
                   std::vector<std::wstring>* values) const;

    // Reads raw data, and |data_type| indicates the what the type of the data is.
    bool ReadValue(const wchar_t* value_name, DWORD restricted_type, void* data,
                   DWORD* data_size) const;
    bool ReadValue(const wchar_t* value_name, void* data, DWORD* data_size,
                   DWORD* data_type) const;

    // For each of the following write functions, it throws an exception when it
    // fails.

    // Sets a REG_DWORD registry field.
    void WriteValue(const wchar_t* value_name, DWORD value);

    // Sets a REG_QWORD registry field.
    void WriteValue(const wchar_t* value_name, DWORD64 value);

    // Sets a REG_SZ registry field.
    void WriteValue(const wchar_t* value_name, const wchar_t* value);

    // Sets a raw data into a specific registry field.
    void WriteValue(const wchar_t* value_name, const void* data, DWORD data_size,
                    DWORD data_type);

private:
    HKEY key_;
};

class RegKeyIterator {
public:
    RegKeyIterator(HKEY rootkey, const wchar_t* folder_key);

    ~RegKeyIterator();

    // Disallow copy.
    RegKeyIterator(const RegKeyIterator&) = delete;
    RegKeyIterator& operator=(const RegKeyIterator&) = delete;

    // Support for move-semantics
    RegKeyIterator(RegKeyIterator&& other);
    RegKeyIterator& operator=(RegKeyIterator&& other);

private:
    HKEY key_;
    size_t index_;
};

class RegValueIterator {

};

}   // namespace kbase

#endif  // KBASE_REGISTRY_H_