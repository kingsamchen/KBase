/*
 @ Kingsley Chen
*/

#include "kbase\registry.h"

#include <cassert>

#include "kbase\error_exception_util.h"

namespace kbase {

RegKey::RegKey()
    : key_(nullptr)
{}

RegKey::RegKey(HKEY key)
    : key_(key)
{}

RegKey::RegKey(HKEY rootkey, const wchar_t* subkey, REGSAM access)
    : key_(nullptr)
{
    if (rootkey) {
        Create(rootkey, subkey, access);
    } else {
        assert(!subkey);
    }
}

RegKey::~RegKey()
{
    Close();
}

void RegKey::Create(HKEY rootkey, const wchar_t* subkey, REGSAM access)
{
    Create(rootkey, subkey, access, nullptr);
}

void RegKey::Create(HKEY rootkey, const wchar_t* subkey, REGSAM access,
                    DWORD* disposition)
{
    assert(rootkey && subkey && access);
    Close();

    long result = RegCreateKeyEx(rootkey, subkey, 0, nullptr, REG_OPTION_NON_VOLATILE,
                                 access, nullptr, &key_, disposition);
    ThrowLastErrorIf(result != ERROR_SUCCESS, "cannot create or open the key!");
}

void RegKey::CreateKey(const wchar_t* key_name, REGSAM access)
{
    assert(key_name && access);

    HKEY subkey = nullptr;
    long result = RegCreateKeyEx(key_, key_name, 0, nullptr, REG_OPTION_NON_VOLATILE,
                                 access, nullptr, &subkey, nullptr);
    ThrowLastErrorIf(result != ERROR_SUCCESS, "cannot create or open the key!");

    // We no longer need the old key handle.
    Close();

    key_ = subkey;
}

void RegKey::Open(HKEY rootkey, const wchar_t* subkey, REGSAM access)
{
    assert(rootkey && subkey && access);
    Close();

    long result = RegOpenKeyEx(rootkey, subkey, 0, access, &key_);
    ThrowLastErrorIf(result != ERROR_SUCCESS, "cannot open the key!");
}

void RegKey::OpenKey(const wchar_t* key_name, REGSAM access)
{
    assert(key_name && access);

    HKEY subkey = nullptr;
    long result = RegOpenKeyEx(key_, key_name, 0, access, &subkey);
    ThrowLastErrorIf(result != ERROR_SUCCESS, "cannot open the key!");

    Close();

    key_ = subkey;
}

void RegKey::Close()
{
    // TODO: stop watching
    if (key_) {
        RegCloseKey(key_);
        key_ = nullptr;
    }
}

}   // namespace kbase