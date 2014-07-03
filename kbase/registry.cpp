/*
 @ Kingsley Chen
*/

#include "kbase\registry.h"

#include <cassert>
#include <cstdlib>

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

RegKey::RegKey(RegKey&& other)
    : key_(nullptr)
{
    *this = std::move(other);
}

RegKey& RegKey::operator=(RegKey&& other)
{
    if (this != &other) {
        key_ = other.key_;
        other.key_ = nullptr;
    }

    return *this;
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
    // It seems that Reg* API series does not set calling thread's last error code
    // when they fail.
    SetLastError(result);
    ThrowLastErrorIf(result != ERROR_SUCCESS, "cannot create or open the key!");
}

void RegKey::CreateKey(const wchar_t* key_name, REGSAM access)
{
    assert(key_name && access);

    HKEY subkey = nullptr;
    long result = RegCreateKeyEx(key_, key_name, 0, nullptr, REG_OPTION_NON_VOLATILE,
                                 access, nullptr, &subkey, nullptr);
    SetLastError(result);
    ThrowLastErrorIf(result != ERROR_SUCCESS, "cannot create or open the key!");

    Close();

    key_ = subkey;
}

void RegKey::Open(HKEY rootkey, const wchar_t* subkey, REGSAM access)
{
    assert(rootkey && subkey && access);
    Close();

    long result = RegOpenKeyEx(rootkey, subkey, 0, access, &key_);
    SetLastError(result);
    ThrowLastErrorIf(result != ERROR_SUCCESS, "cannot open the key!");
}

void RegKey::OpenKey(const wchar_t* key_name, REGSAM access)
{
    assert(key_name && access);

    HKEY subkey = nullptr;
    long result = RegOpenKeyEx(key_, key_name, 0, access, &subkey);
    SetLastError(result);
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

bool RegKey::HasValue(const wchar_t* value_name) const
{
    BOOL result = RegQueryValueEx(key_, value_name, 0, nullptr, nullptr, nullptr);

    if (result == ERROR_SUCCESS) {
        return true;
    }

    SetLastError(result);
    LastError err;
    ThrowLastErrorIf(err.last_error_code() != ERROR_FILE_NOT_FOUND, "error occured");
    
    return false;
}

size_t RegKey::GetValueCount() const
{
    DWORD value_count = 0;
    long result = RegQueryInfoKey(key_, nullptr, nullptr, nullptr, nullptr, nullptr,
                                  nullptr, &value_count, nullptr, nullptr, nullptr,
                                  nullptr);

    SetLastError(result);
    ThrowLastErrorIf(result != ERROR_SUCCESS, "failed to get value count");
    
    return static_cast<size_t>(value_count);
}

void RegKey::GetValueNameAt(size_t index, std::wstring* value_name) const
{
    wchar_t buf[MAX_PATH];
    DWORD buf_size = _countof(buf);

    long result = RegEnumValue(key_, index, buf, &buf_size, nullptr, nullptr, nullptr,
                               nullptr);
    SetLastError(result);
    ThrowLastErrorIf(result != ERROR_SUCCESS, "failed to get value name");

    *value_name = buf;
}

void RegKey::DeleteKey(const wchar_t* key_name)
{
    assert(key_ && key_name);

    long result = RegDeleteTree(key_, key_name);
    SetLastError(result);
    ThrowLastErrorIf(result != ERROR_SUCCESS, "failed to delete the key");
}

void RegKey::DeleteValue(const wchar_t* value_name)
{
    assert(key_ && value_name);

    long result = RegDeleteValue(key_, value_name);
    SetLastError(result);
    ThrowLastErrorIf(result != ERROR_SUCCESS, "failed to delete the value");
}

}   // namespace kbase