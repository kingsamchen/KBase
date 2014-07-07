/*
 @ Kingsley Chen
*/

#include "kbase\registry.h"

#include <cassert>
#include <cstdlib>

#include "kbase\error_exception_util.h"
#include "kbase\strings\string_util.h"

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

HKEY RegKey::Release()
{
    HKEY key = key_;
    key_ = nullptr;

    return key;
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

bool RegKey::ReadValue(const wchar_t* value_name, void* data, DWORD* data_size,
                       DWORD* data_type) const
{
    long result = RegGetValue(key_, nullptr, value_name, 0, data_type, data,
                              data_size);

    return result == ERROR_SUCCESS ? true : (SetLastError(result), false);
}

bool RegKey::ReadValue(const wchar_t* value_name, DWORD restricted_type, void* data,
                       DWORD* data_size) const
{
    long result = RegGetValue(key_, nullptr, value_name, restricted_type, nullptr,
                              data, data_size);

    return result == ERROR_SUCCESS ? true : (SetLastError(result), false);
}

bool RegKey::ReadValue(const wchar_t* value_name, std::wstring* value) const
{
    const size_t kCharSize = sizeof(wchar_t);
    size_t str_length = 1024;   // including null
    // It seems that automatic expansion for environment strings in RegGetValue 
    // behaves incorrect when using std::basic_string as its buffer. 
    // Therefore, does expansions on our own.
    DWORD restricted_type = RRF_RT_REG_SZ | RRF_NOEXPAND | RRF_RT_REG_EXPAND_SZ;
    DWORD data_type = 0;
    DWORD data_size = 0;
    std::wstring raw_data;

    long result = 0;
    do {
        wchar_t* data_ptr = WriteInto(&raw_data, str_length);
        data_size = str_length * kCharSize;
        result = RegGetValue(key_, nullptr, value_name, restricted_type, &data_type,
                             data_ptr, &data_size);
        if (result == ERROR_SUCCESS) {
            if (data_type == REG_SZ) {
                size_t written_length = data_size / kCharSize - 1;  
                value->assign(data_ptr, written_length);
                return true;
            } else if (data_type == REG_EXPAND_SZ) {
                std::wstring expanded;
                wchar_t* ptr = WriteInto(&expanded, str_length);
                size_t size = ExpandEnvironmentStrings(data_ptr, ptr, str_length);
                if (size == 0) {
                    // functions fails, and it internally sets the last error.
                    return false;
                } else if (size > str_length) {
                    data_size = size * kCharSize;
                    result = ERROR_MORE_DATA;
                } else {
                    value->assign(ptr, size - 1);
                    return true;
                }
            }
        }
    } while (result == ERROR_MORE_DATA && (str_length = data_size / kCharSize, true));

    // An error caused by registry APIs occured.
    SetLastError(result);
    return false;
}

bool RegKey::ReadValue(const wchar_t* value_name,
                              std::vector<std::wstring>* values) const
{
    assert(values);
    const size_t kCharSize = sizeof(wchar_t);
    DWORD restricted_type = RRF_RT_REG_MULTI_SZ;
    DWORD data_size = 0;

    // gets the data size, in bytes.
    bool result = ReadValue(value_name, restricted_type, nullptr, &data_size);
    if (!result) {
        return false;
    }

    std::wstring raw_data;
    wchar_t* data_ptr = WriteInto(&raw_data, data_size / kCharSize);
    result = ReadValue(value_name, restricted_type, data_ptr, &data_size);
    if (!result) {
        return false;
    }

    Tokenize(raw_data, std::wstring(1, 0), values);
    
    return true;
}

bool RegKey::ReadValue(const wchar_t* value_name, DWORD* value) const
{
    assert(value);
    DWORD restricted_type = RRF_RT_DWORD;
    DWORD tmp = 0;
    DWORD data_size = sizeof(DWORD);

    bool result = ReadValue(value_name, restricted_type, &tmp, &data_size);
    if (!result) {
        return false;
    }

    assert(data_size == sizeof(DWORD));
    *value = tmp;

    return true;
}

bool RegKey::ReadValue(const wchar_t* value_name, DWORD64* value) const
{
    assert(value);
    DWORD restricted_type = RRF_RT_QWORD;
    DWORD64 tmp = 0;
    DWORD data_size = sizeof(DWORD64);

    bool result = ReadValue(value_name, restricted_type, &tmp, &data_size);
    if (!result) {
        return false;
    }

    assert(data_size == sizeof(DWORD64));
    *value = tmp;

    return true;
}

void RegKey::WriteValue(const wchar_t* value_name, DWORD value)
{
    WriteValue(value_name, &value, sizeof(DWORD), REG_DWORD);
}

void RegKey::WriteValue(const wchar_t* value_name, DWORD64 value)
{
    WriteValue(value_name, &value, sizeof(DWORD64), REG_QWORD);
}

void RegKey::WriteValue(const wchar_t* value_name, const wchar_t* value)
{
    WriteValue(value_name, value, sizeof(wchar_t) * (wcslen(value) + 1), REG_SZ);
}

void RegKey::WriteValue(const wchar_t* value_name, const void* data, DWORD data_size,
                        DWORD data_type)
{
    assert(data && data_size > 0);
    long result = RegSetValueEx(key_, value_name, 0, data_type,
                                static_cast<const BYTE*>(data), data_size);
    SetLastError(result);
    ThrowLastErrorIf(result != ERROR_SUCCESS, "failed to write value");
}

// RegKeyIterator class implementations.

RegKeyIterator::RegKeyIterator(HKEY rootkey, const wchar_t* folder_key)
{

}

RegKeyIterator::~RegKeyIterator()
{
}

}   // namespace kbase