/*
 @ 0xCCCCCCCC
*/

#include "kbase/registry.h"

#include <cassert>

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"
#include "kbase/string_encoding_conversions.h"
#include "kbase/string_util.h"

namespace kbase {

RegKey::RegKey() noexcept
    : key_(nullptr)
{}

RegKey::RegKey(HKEY rootkey, const wchar_t* subkey, REGSAM access, DWORD& disposition)
    : key_(nullptr)
{
    auto rv = RegCreateKeyExW(rootkey, subkey, 0, nullptr, REG_OPTION_NON_VOLATILE, access,
                                  nullptr, &key_, &disposition);

    LOG_IF(WARNING, rv != ERROR_SUCCESS)
        << "Failed to create instance for key " << kbase::WideToUTF8(subkey)
        << "; Error: " << rv;
}

RegKey::RegKey(RegKey&& other) noexcept
    : key_(other.Release())
{}

RegKey& RegKey::operator=(RegKey&& other) noexcept
{
    Close();
    key_ = other.Release();

    return *this;
}

RegKey::~RegKey()
{
    Close();
}

// static
RegKey RegKey::Create(HKEY rootkey, const wchar_t* subkey, REGSAM access)
{
    DWORD ignored;
    return Create(rootkey, subkey, access, ignored);
}

// static
RegKey RegKey::Create(HKEY rootkey, const wchar_t* subkey, REGSAM access, DWORD& disposition)
{
    ENSURE(CHECK, subkey != nullptr).Require();
    return RegKey(rootkey, subkey, access, disposition);
}

void RegKey::Open(const wchar_t* subkey, REGSAM access)
{
    Open(key_, subkey, access);
}

void RegKey::Open(HKEY rootkey, const wchar_t* subkey, REGSAM access)
{
    ENSURE(CHECK, rootkey != nullptr && subkey != nullptr).Require();

    HKEY new_key = nullptr;
    auto rv = RegOpenKeyExW(rootkey, subkey, 0, access, &new_key);
    LOG_IF(WARNING, rv != ERROR_SUCCESS)
        << "Failed to create instance for key " << kbase::WideToUTF8(subkey)
        << "; Error: " << rv;

    Close();

    key_ = new_key;
}

HKEY RegKey::Get() const noexcept
{
    return key_;
}

HKEY RegKey::Release() noexcept
{
    HKEY key = key_;
    key_ = nullptr;

    return key;
}

void RegKey::Close() noexcept
{
    if (key_) {
        RegCloseKey(key_);
        key_ = nullptr;
    }
}

// static
bool RegKey::KeyExists(HKEY rootkey, const wchar_t* subkey, WOW6432Node node_mode)
{
    REGSAM access = KEY_READ;
    if (node_mode == Force32KeyOnWOW64) {
        access |= KEY_WOW64_32KEY;
    } else if (node_mode == Force64KeyOnWOW64) {
        access |= KEY_WOW64_64KEY;
    }

    HKEY key = nullptr;
    auto rv = RegOpenKeyExW(rootkey, subkey, 0, access, &key);
    if (rv == ERROR_SUCCESS) {
        RegCloseKey(key);
        return true;
    }

    ENSURE(RAISE, rv == ERROR_FILE_NOT_FOUND)(rv).Require();

    return false;
}

bool RegKey::HasValue(const wchar_t* value_name) const
{
    ENSURE(RAISE, IsValid()).Require();

    auto rv = RegQueryValueExW(key_, value_name, nullptr, nullptr, nullptr, nullptr);
    if (rv == ERROR_SUCCESS) {
        return true;
    }

    ENSURE(RAISE, rv == ERROR_FILE_NOT_FOUND)(rv).Require();

    return false;
}

size_t RegKey::GetValueCount() const
{
    ENSURE(RAISE, IsValid()).Require();

    DWORD value_count = 0;
    auto rv = RegQueryInfoKeyW(key_, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                               &value_count, nullptr, nullptr, nullptr, nullptr);

    ENSURE(RAISE, rv == ERROR_SUCCESS)(rv).Require();

    return static_cast<size_t>(value_count);
}

void RegKey::GetValueNameAt(size_t index, std::wstring& value_name) const
{
    ENSURE(RAISE, IsValid()).Require();

    wchar_t buf[MAX_PATH + 1];
    DWORD buf_size = _countof(buf);
    auto rv = RegEnumValueW(key_, static_cast<DWORD>(index), buf, &buf_size, nullptr, nullptr,
                            nullptr, nullptr);

    ENSURE(RAISE, rv == ERROR_SUCCESS)(rv).Require();

    value_name = buf;
}

void RegKey::DeleteKey(const wchar_t* key_name) const
{
    ENSURE(RAISE, IsValid()).Require();

    auto rv = RegDeleteTreeW(key_, key_name);
    ENSURE(RAISE, rv == ERROR_SUCCESS)(rv).Require();
}

void RegKey::DeleteValue(const wchar_t* value_name) const
{
    ENSURE(RAISE, IsValid()).Require();

    auto rv = RegDeleteValueW(key_, value_name);
    ENSURE(RAISE, rv == ERROR_SUCCESS)(rv).Require();
}

void RegKey::ReadRawValue(const wchar_t* value_name, void* data, DWORD& data_size,
                          DWORD& data_type) const
{
    ENSURE(RAISE, IsValid()).Require();

    auto rv = RegGetValueW(key_, nullptr, value_name, 0, &data_type, data, &data_size);
    ENSURE(RAISE, rv == ERROR_SUCCESS)(rv).Require();
}

void RegKey::ReadRawValue(const wchar_t* value_name, DWORD restricted_type, void* data,
                          DWORD& data_size) const
{
    ENSURE(RAISE, IsValid()).Require();

    auto rv = RegGetValueW(key_, nullptr, value_name, restricted_type, nullptr, data, &data_size);
    ENSURE(RAISE, rv == ERROR_SUCCESS)(rv).Require();
}

void RegKey::ReadValue(const wchar_t* value_name, std::wstring& value) const
{
    ENSURE(RAISE, IsValid()).Require();

    constexpr DWORD kCharSize = sizeof(wchar_t);

    // Length including null.
    DWORD str_length = 1024;

    // It seems that automatic expansion for environment strings in RegGetValue
    // behaves incorrect when using std::basic_string as its buffer.
    // Therefore, does expansions on our own.
    DWORD restricted_type = RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND;
    DWORD data_type = 0;
    DWORD data_size = 0;

    std::wstring raw_data;

    long rv = 0;
    do {
        wchar_t* data_ptr = WriteInto(raw_data, str_length);
        data_size = str_length * kCharSize;
        rv = RegGetValueW(key_, nullptr, value_name, restricted_type, &data_type, data_ptr,
                          &data_size);
        if (rv == ERROR_SUCCESS) {
            if (data_type == REG_SZ) {
                size_t written_length = data_size / kCharSize - 1;
                value.assign(data_ptr, written_length);
                return;
            }

            if (data_type == REG_EXPAND_SZ) {
                std::wstring expanded;
                wchar_t* ptr = WriteInto(expanded, str_length);
                DWORD size = ExpandEnvironmentStringsW(data_ptr, ptr, str_length);
                ENSURE(RAISE, size > 0)(LastError()).Require();
                if (size > str_length) {
                    data_size = size * kCharSize;
                    rv = ERROR_MORE_DATA;
                } else {
                    value.assign(ptr, size - 1);
                    return;
                }
            }
        }
    } while (rv == ERROR_MORE_DATA && (str_length = data_size / kCharSize, true));

    ENSURE(RAISE, NotReached())(rv).Require();
}

void RegKey::ReadValue(const wchar_t* value_name, std::vector<std::wstring>& values) const
{
    ENSURE(RAISE, IsValid()).Require();

    constexpr size_t kCharSize = sizeof(wchar_t);
    DWORD restricted_type = RRF_RT_REG_MULTI_SZ;
    DWORD data_size = 0;

    // Acquires the data size, in bytes.
    ReadRawValue(value_name, restricted_type, nullptr, data_size);

    std::wstring raw_data;
    wchar_t* data_ptr = WriteInto(raw_data, data_size / kCharSize);
    ReadRawValue(value_name, restricted_type, data_ptr, data_size);

    SplitString(raw_data, std::wstring(1, L'\0'), values);
}

void RegKey::ReadValue(const wchar_t* value_name, DWORD& value) const
{
    DWORD restricted_type = RRF_RT_DWORD;
    DWORD tmp = 0;
    DWORD data_size = sizeof(DWORD);

    ReadRawValue(value_name, restricted_type, &tmp, data_size);

    ENSURE(CHECK, data_size == sizeof(DWORD))(data_size).Require();

    value = tmp;
}

void RegKey::ReadValue(const wchar_t* value_name, DWORD64& value) const
{
    DWORD restricted_type = RRF_RT_QWORD;
    DWORD64 tmp = 0;
    DWORD data_size = sizeof(DWORD64);

    ReadRawValue(value_name, restricted_type, &tmp, data_size);

    ENSURE(CHECK, data_size == sizeof(DWORD64))(data_size).Require();

    value = tmp;
}

void RegKey::WriteValue(const wchar_t* value_name, DWORD value) const
{
    WriteValue(value_name, &value, sizeof(DWORD), REG_DWORD);
}

void RegKey::WriteValue(const wchar_t* value_name, DWORD64 value) const
{
    WriteValue(value_name, &value, sizeof(DWORD64), REG_QWORD);
}

void RegKey::WriteValue(const wchar_t* value_name, const wchar_t* value, size_t length) const
{
    // Data size includes terminating-null character.
    WriteValue(value_name, value, sizeof(wchar_t) * (length + 1), REG_SZ);
}

void RegKey::WriteValue(const wchar_t* value_name, const void* data, size_t data_size,
                        DWORD data_type) const
{
    ENSURE(CHECK, data && data_size > 0).Require();

    auto rv = RegSetValueExW(key_, value_name, 0, data_type, static_cast<const BYTE*>(data),
                             static_cast<DWORD>(data_size));

    ENSURE(RAISE, rv == ERROR_SUCCESS)(rv).Require();
}

// RegKeyIterator class implementations.

RegKeyIterator::RegKeyIterator(HKEY rootkey, const wchar_t* folder_key)
    : key_(nullptr), index_(-1), subkey_count_(0)
{
    long result = RegOpenKeyEx(rootkey, folder_key, 0, KEY_READ, &key_);
    if (result == ERROR_SUCCESS) {
        DWORD subkey_count = 0;
        result = RegQueryInfoKey(key_, nullptr, nullptr, nullptr, &subkey_count,
                                 nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                 nullptr);
        if (result != ERROR_SUCCESS) {
            Close();
        } else {
            subkey_count_ = subkey_count;
            index_ = subkey_count - 1;
        }
    }

    Read();
}

RegKeyIterator::~RegKeyIterator()
{
    Close();
}

RegKeyIterator::RegKeyIterator(RegKeyIterator&& other)
{
    *this = std::move(other);
}

RegKeyIterator& RegKeyIterator::operator=(RegKeyIterator&& other)
{
    if (this != &other) {
        key_ = other.key_;
        index_ = other.index_;

        other.key_ = nullptr;
        other.index_ = -1;

        key_name_ = std::move(other.key_name_);
    }

    return *this;
}

void RegKeyIterator::Close()
{
    if (key_) {
        RegCloseKey(key_);
        key_ = nullptr;
    }
}

bool RegKeyIterator::Read()
{
    if (Valid()) {
        DWORD name_length = MAX_PATH;
        long result = RegEnumKeyEx(key_, index_, key_name_.data(), &name_length,
                                   nullptr, nullptr, nullptr, nullptr);
        if (result == ERROR_SUCCESS) {
            return true;
        }
    }

    key_name_[0] = L'\0';
    return false;
}

RegKeyIterator& RegKeyIterator::operator++()
{
    if (Valid()) {
        --index_;
        Read();
    }

    return *this;
}

// RegValueIterator class implementations

RegValueIterator::RegValueIterator(HKEY rootkey, const wchar_t* folder_key)
    : key_(nullptr),
      index_(-1),
      value_count_(0),
      value_(INITIAL_VALUE_SIZE, 0),
      value_size_(0)
{
    long result = RegOpenKeyEx(rootkey, folder_key, 0, KEY_READ, &key_);
    if (result == ERROR_SUCCESS) {
        DWORD value_count = 0;
        DWORD max_value_name_length = 0, max_value_length = 0;
        result = RegQueryInfoKey(key_, nullptr, nullptr, nullptr, nullptr, nullptr,
                                 nullptr, &value_count, &max_value_name_length,
                                 &max_value_length, nullptr, nullptr);
        if (result != ERROR_SUCCESS) {
            Close();
        } else {
            value_count_ = value_count;
            index_ = value_count - 1;
            max_value_name_length_ = max_value_name_length;
            max_value_length_ = max_value_length;
        }
    }

    Read();
}

RegValueIterator::~RegValueIterator()
{
    Close();
}

RegValueIterator::RegValueIterator(RegValueIterator&& other)
{
    *this = std::move(other);
}

RegValueIterator& RegValueIterator::operator=(RegValueIterator&& other)
{
    if (this != &other) {
        key_ = other.key_;
        index_ = other.index_;
        value_count_ = other.value_count_;
        type_ = other.type_;
        value_size_ = other.value_size_;
        value_name_ = std::move(other.value_name_);
        value_ = std::move(other.value_);

        other.key_ = nullptr;
        other.index_ = -1;
        other.value_count_ = 0;
        other.type_ = REG_NONE;
        other.value_size_ = 0;
    }

    return *this;
}

void RegValueIterator::Close()
{
    if (key_) {
        RegCloseKey(key_);
        key_ = nullptr;
    }
}

RegValueIterator& RegValueIterator::operator++()
{
    if (Valid()) {
        --index_;
        Read();
    }

    return *this;
}

bool RegValueIterator::Read()
{
    if (Valid()) {
        DWORD value_size = INITIAL_VALUE_SIZE;
        DWORD name_length = INITIAL_NAME_SIZE;
        wchar_t* name = WriteInto(value_name_, name_length);
        long result = RegEnumValue(key_, index_, name, &name_length, nullptr, &type_,
                                   reinterpret_cast<BYTE*>(&value_[0]), &value_size);
        // Current size is too small.
        if (result == ERROR_MORE_DATA) {
            assert(value_size < max_value_length_);
            assert(name_length < max_value_name_length_);

            value_size = max_value_length_;
            name_length = max_value_name_length_;
            name = WriteInto(value_name_, name_length);
            value_.resize(value_size, 0);

            result = RegEnumValue(key_, index_, name, &name_length, nullptr, &type_,
                                  reinterpret_cast<BYTE*>(&value_[0]), &value_size);
        }

        if (result == ERROR_SUCCESS) {
            value_size_ = value_size;
            return true;
        }
    }

    type_ = REG_NONE;
    value_name_[0] = L'\0';
    value_[0] = 0;
    value_size_ = 0;
    return false;
}

}   // namespace kbase
