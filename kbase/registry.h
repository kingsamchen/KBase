/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_REGISTRY_H_
#define KBASE_REGISTRY_H_

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

    void OpenKey(const wchar_t* key_name, REGSAM access);

    void Close();

private:
    HKEY key_;
};

class RegKeyIterator {

};

class RegValueIterator {

};

}   // namespace kbase

#endif  // KBASE_REGISTRY_H_