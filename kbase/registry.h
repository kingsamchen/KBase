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

class RegKey {
public:
    RegKey();
    
    explicit RegKey(HKEY key);
    
    RegKey(HKEY root, const wchar_t* subkey, REGSAM access);
    
    ~RegKey();

    RegKey(const RegKey&) = delete;

    RegKey& operator=(const RegKey&) = delete;

    long Create(HKEY root, const wchar_t* subkey, REGSAM access,
                DWORD* disposition = nullptr);

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