/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_REGISTRY_H_
#define KBASE_REGISTRY_H_

#include <string>

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

    RegKey(RegKey&& other);

    RegKey& operator=(RegKey&& other);

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

private:
    HKEY key_;
};

class RegKeyIterator {

};

class RegValueIterator {

};

}   // namespace kbase

#endif  // KBASE_REGISTRY_H_