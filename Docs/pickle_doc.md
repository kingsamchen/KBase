Pickle and PickleIterator
===

---

[TOC]


## Pickle

`Pickle` is a light-weight data serialization facility.

It supports serialization/deserialization for built-in data types and a chunk of buffer with size specified.

`Pickle` internally use a buffer for stashing *header* and storing *payload* data.

```
<------------capacity----------->
+------+----+----+---+----+-----+
|header|seg1|seg2|...|segn|     |
+------+----+----+---+----+-----+
       <-----payload------>
```

Each segment in payload begins on the memory address that is on 32-bit boundary, thus there might/must be a gap between two logically consective segments.

**NOTE:** `Pickle` is only responsible for data serialization; `PickleIterator` is what you need when you want to have your data reinstated.

### ctor/dtor/operator=

---

#### Pickle()

Constructs a `Pickle` object with no payload attached.

#### Pickle(const char* data, int data_len)

Constructs a `Pickle` object that has *weak-reference* to an existing serialized buffer.

**NOTE:** The object created in this form does not allocate memory for its internal buffer.

The `Pickle` object **cannot** call any **modifiable** methods (if you do do this, you would recieve assert failed on debug mode, whereas in release mode, methods merely return `false`), and caller must ensure the referee is in a valid state, when PickleIterator is applied.

This constructor is mainly for when multiple `PickleIterator` objects that read on a same buffer is required.

```c++
std::vector<char> buf;
Pickle pickle;
// do some serialization, and now buf contains marshalled data

PickleIterator it1(Pickle(buf.data(), buf.size()));
// do sth...
PickleIterator it2(Pickle(buf.data(), buf.size()));
// do sth...
```

#### Pickle(const Pickle& other)

Makes a deep-copy of other `Pickle` object.

If the `other` is constructed from a constant buffer, the newly created object still allocates an internal buffer and copy everthing into from the `other`.

#### Pickle(Pickle&& other)

Move-constructor.

#### Pickle& operator=(const Pickle& rhs)

Makes a deep-copy of `rhs`.

It's behavior is similar to copy-constructor.

#### Pickle& operator=(Pickle&& rhs)

Move-assignment

#### ~Pickle()

If the `Pickle` object is not constructed from a buffer, it will release the internal buffer in destructor.

### Size and Data

---

#### size_t size() [*const*]

Returns the size of internal data, including header, in bytes.

Whenever you use a `Pickle` object, its `size()` must not be zero, since the header is always counted.

#### bool empty() [*const*]

Checks if the payload is empty. Thus, returns `true` if no payload there; otherwise, return `false`.

#### size_t payload_size() [*const*]

Returns the size of payload, in bytes.

#### const void\* data() [*const*]

Returns pointer to the internal buffer.

#### const char\* payload() [*const*]

Returns pointer to the address where payload begins on.

### Serialization

---

**NOTE:** The following functions guarantee that the internal data remains unchanged when they fail.

#### bool Write(T value)

Serializes value of type T into the internal buffer.

**NOTE:** `T` here is only a placeholder for types of the real functions.

#### bool WriteByte(const void* data, int data_len)

Serializes data in `data_len` bytes.

Be wary of that the `data_len` would not be written into the object. You must guarantee you have the real size of this data chunk, when you use `PickleIterator` to reinstate it back into memory.

```c++
const char kStr[] = "helloworld";   // 10-byte, not including NULL-terminator
kbase::Pickle pickle;
pickle.WriteByte(kStr, 10);
std::cout << pickle.payload_size(); // -> 10
```

#### bool WriteData(const char* data, int length)

Serializes data **along with** its `length`. `length` is ahead of data.

```c++
const char kStr[] = "helloworld";   // 10-byte, not including NULL-terminator
kbase::Pickle pickle;
pickle.WriteData(kStr, 10);
std::cout << pickle.payload_size(); // -> 4 + 10
```

## PickleIterator

Deserializes data from a `Pickle`.

Any function of `PickleIterator` do not modify the state or buffer of `Pickle`.

Be wary of that, `PickleIterator` is logically a *forward-iterator*. It has no ability to read-or-move backward.

### ctor/dtor/operato=

---

#### PickleIterator(const Pickle& pickle) [*explicit*]

Constructs a `PickleIterator` object from a `Pickle` object.

For now, it is the only way you could create a `PickleIterator` object.

### Deserialization

---

#### bool Read(T* result)

Read data of type `T` off the serialized data.

If function fails, the variable `result` points to remains unchanged.

#### bool ReadBytes(const char** data, int length)

Read a chunk of data with size `length`.

If the function fails, the pointer `*data` will be set as `NULL`.

**NOTE:** If the function succeeds, the `*data` points to an address within the internal buffer, and you **shall never** do any modifiable operations with this pointer. Instead, you should make yourself a new buffer then copy data into.

```c++
const char kStr[] = "helloworld";   // 10-byte, not including NULL-terminator
kbase::Pickle pickle;
pickle.WriteBytes(kStr, 10);

kbase::PickleIterator pickle_it(pickle);
const char* data = nullptr;
int data_size = 10;
if (pickle_it.ReadData(&data, data_size)) {
    std::vector<char> v(data, data + data_size);
    // do sth with v.
}
```

#### bool ReadData(const char** data, int* read_length)

Similar to `ReadBytes`.

See minor difference at `WriteBytes` and `WriteData`.

#### bool SkipBytes(int num_bytes)

move internal read-pointer forward `num_bytes`.

If the function fails, the internal read-pointer remains unchanged. i.e. the skip as though never happened.