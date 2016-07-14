# Pickle and PickleReader

[TOC]

### Motivation

`Pickle` is a handy tool for serialization of data in memory, which supports primitives, string, common STL containers, and even custom classes.

`PickleReader` does the work way around, i.e. it does deserialization from packed memory data.

In fact, you can store data pickled on the disk to make it persistent, and then restore it later.



### For Common Types

`Pickle` overloads `operator<<` for packing, while `PickleReader` overloads `operator>>` for unpacking.

Note that, a `PickleReader` instance won't affect state, or content, of a `Pickle` instance it refers to.

```c++
int i = 123;
std::string s = "hello world";
std::vector<double> vd {1., 3.14, 2.99};

kbase::Pickle pickle;
pickle << t << s << vd;

...

kbase::PickleReader reader(pickle); // no matter what reader does, it will not change pickle.
int ci;
std::string cs;
std::vector<double> cvd;
reader >> ci >> cs >> cvd;

assert(i == ci);
assert(s == cs);
assert(vd == cvd);
```



### For Custom Classes

A custom class that wants to be pickle-able, should satisfy the following requirements:

- the class adds a global overload operator<< for Pickle
- the class adds a global overload operator>> for PickleReader
- the class itself can be default-constructed
- members in the class are accessible to Pickle and PickleReader

```c++
class Foo {
public:
    Foo();

private:
    friend class Pickle;
    friend class PickleReader;
    ...
};

kbase::Pickle& operator<<(kbase::Pickle& pickle, const Foo& foo)
{
    // pickle members in foo
    return pickle;
}

kbase::PickleReader operator>>(kbase::PickleReader& reader, Foo& foo)
{
    // un-pickle for foo
    return reader;
}
```



### Pickled Buffer

You have access to the buffer previsouly pickled, and you can create a `PickleReader` instance from this buffer.

```c++
std::vector<std::string> ss { "hello world", "this is", "pickle" };
kbase::Pickle pickle;
pickle << ss;
...
std::vector<uint8_t> buf(ss.size());    // or this buf can be read from storage.
memcpy(buf.data(), ss.data(), ss.size());
kbase::PickleReader reader(buf.data(), buf.size());
...
```