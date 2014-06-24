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

RegKey::RegKey(HKEY root, const wchar_t* subkey, REGSAM access)
    : key_(nullptr)
{
    if (root) {
        long result = Create(root, subkey, access);
        ThrowLastErrorIf(result != ERROR_SUCCESS, "cannot create or open the key!");
    } else {
        assert(!subkey);
    }
}

RegKey::~RegKey()
{
    Close();
}

long RegKey::Create(HKEY root, const wchar_t* subkey, REGSAM access,
                    DWORD* disposition /* = nullptr*/)
{
    assert(root && subkey && access);
    Close();

    long result = RegCreateKeyEx(root, subkey, 0, nullptr, REG_OPTION_NON_VOLATILE,
                                 access, nullptr, &key_, disposition);

    return result;
}

void RegKey::Close()
{
    if (key_) {
        RegCloseKey(key_);
        key_ = nullptr;
    }
}

}   // namespace kbase