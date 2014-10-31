AutoReset
===

An `AutoReset` object can guard one variable's value. Every modifications to the variable will be rolled back once the `AutoReset` object goes out of scope.

Be wary of that, an AutoReset object must have shorter lifetime than the scoped_var object. Otherwise, it may cause invalid memory reference during destruction.

Moreover, to ensure the destructor of `AutoReset` never throw an exception, protectee must make sure no exception would leak out of its move-assignment and copy-assignment.

```c++
std::vector<std::string> origin {"hello", "world", "kc"};
std::vector<std::string> v = origin;
{
    AutoReset<decltype(v)> value_guard(&v);
    v.pop_back();
    v.push_back("this is a test");
    EXPECT_NE(origin, v);
}
EXPECT_EQ(origin, v);
```