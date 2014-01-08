KBase_Demo
==========

some imitations of a series of remarkable infrastructures.

some will be re-written with C++11 features; and the most will be modified to adapt to my personal needs.

### AtExitManager

provide a facility similar to the CRT `atexit()`. actually is a imitation of `base::AtExitManager`

the usage is simple. early in the `main()` or `WinMain()` scope create an object on the stack:

```c++
int main()
{
    KBase::AtExitManager exitManager;
}
```

when the object goes out of scope, all registered callbacks and the singleton destructors will be called

### Pickle [to be contiued...]

a light-weight serialization/deserialization facility.

support built-in types, and `std::string` and `std::wstring`.

an illustration of a `Pickle` object memory layout

```
<---------capacity-------->
+------+----+----+---+----+
|header|seg1|seg2|...|segn|
+------+----+----+---+----+
       <-----payload------>
```
