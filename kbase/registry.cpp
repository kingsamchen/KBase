/*
 @ Kingsley Chen
*/

#include "kbase/registry.h"

#include <cassert>

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
    
    } else {
        assert(!subkey);
    }
}

}   // namespace kbase