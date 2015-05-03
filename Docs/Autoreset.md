AutoReset
===

An `AutoReset` object can guard one variable's value. Every modifications to the variable will be reverted once the `AutoReset` object goes out of scope.

The type of the variable that want to be guarded **must** support copy-semantics.

Be wary of that, an `AutoReset` object must have shorter lifetime than the `scoped_var` object. Otherwise, it may cause invalid memory reference during destruction.

Moreover, to ensure the destructor of `AutoReset` never throw an exception, protectee must make sure no exception would leak out of its move-assignment and copy-assignment.

## Usage At Glance

```c++
#include "kbase\auto_reset.h"

std::vector<std::string> vec {"hello", "world", "kc"};
auto v = vec;
{
    AutoReset<decltype(v)> value_guard(&v);
    v.pop_back();
    v.push_back("this is a test");
	// now v's elements are "hello", "world", "this is a test".
  	// do sth else...
}

// all modifications are reverted
// v's elements still are "hello", "world", "kc"
```