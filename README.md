KBase_Demo
==========

### Disclaimer

Some imitations of a collection of remarkable infrastructures.

Some will be re-written with C++11 features; and the majority will be modified to adapt to my personal needs.

Since I wrote these files by using Visual Studio 2013, which uses unportable implementations for some parts of standard library or recommends its own alternatives, such as secured-string-runtime, you may need to replace with standard counterparts on your own, if you want compiled with another compilers.

### AtExitManager

provides a facility similar to the CRT `atexit()`. actually is a imitation of `base::AtExitManager`

the usage is simple. early in the `main()` or `WinMain()` scope create an object on the stack:

```c++
int main()
{
    KBase::AtExitManager exitManager;
}
```

when the object goes out of scope, all registered callbacks and the singleton destructors will be called

### Pickle

a light-weight serialization/deserialization facility.

support built-in types, and `std::string` and `std::wstring`.

an illustration of a `Pickle` object memory layout

```
<-----------capacity------------>
+------+----+----+---+----+-----+
|header|seg1|seg2|...|segn|     |
+------+----+----+---+----+-----+
       <-----payload------>
```

an illumination for basic functionalities

```c++
KBase::Pickle pickle;
// serialize
pickle.WriteInt(0x1234);
pickle.WriteBool(true);
pickle.WriteDouble(3.14159);
pickle.WriteString("hell world");

std::cout << "marshal data complete" << std::endl;

// transmit serialized data if necessary
size_t buffer_size = pickle.size();
std::unique_ptr<char[]> buff(new char[buffer_size]);
memcpy(buff.get(), pickle.data(), buffer_size);

KBase::Pickle pickle_out(buff.get(), buffer_size);

int protocol = 0;
bool flag = false;
double pi = 0.0;
std::string str;

// deserialize
KBase::PickleIterator it(pickle_out);
it.ReadInt(&protocol);
it.ReadBool(&flag);
it.ReadDouble(&pi);
it.ReadString(&str);
```

for complete functions and their usage, see the source code.

### AutoReset

provides a facility that set a given object a new value within a scope, and
restores its original value automatically when goes out of scope.

usage:

```c++
std::string str("hello world");
    
{
    cout << "stage 1:" << str << endl;  // hello world
    KBase::AutoReset<std::string> auto_str(&str, "kingsamchen");
    cout << "stage 2:" << str << endl;  // kingsamchen
}

cout << "stage 3:" << str << endl;  // hello world
```

### StringPiece and WStringPiece

a light-weight referential string class. it internally contains a pointer to 
a raw-string and along with its length.

this class intends to reduce overhead resulted from conversions between stl-strings
and raw-strings.

the actual type of the class is `BasicStringPiece<STRING_TYPE>`, and `StringPiece`
and `WStringPiece` are merely

```c++
typedef BasicStringPiece<std::string> StringPiece;
typedef BasicStringPiece<std::wstring> WStringPiece;
```
respectively.

it provides a large amount of methods compatible with normal strings, including
hash-calculation.

for a complete list of its functionalities, please see the file `string_piece.h`.

### String_Utils

offers a lot of handy string utility functions.

see the file `string_util.h` for great details.