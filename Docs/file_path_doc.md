FilePath
===

[TOC]

`FilePath` is a class that intends to be used whenever a path is required.

It supplies a lot of functions for simplifing path process.

In **KBase**, `FilePath` is almost in every where that a path in raw-string was used.

### ctor/dtor/operator=

---

#### FilePath() = default

Default constructor.

#### FilePath(const FilePath& other)

Copy-constructor.

#### FilePath(FilePath&& other)

Move-Constructor.

#### FilePath(const PathString& path) [*explicit*]

Constructs a `FilePath` object from a given path string.

On Windows, `PathString` is just an alias of `std::wstring`.

#### FilePath& operator=(const FilePath& other)

Copy-assignment.

#### FilePath& operator=(FilePath&& other)

Move-assignment.

#### ~FilePath() = default

Default destructor.

### Comparisons

---

On Windows, file path is by default case-insensitive.

Comparisons here do not take local-sensitive into account, and the characters are compared in lexical-order.

**NOTE:** The reason `FilePath` provides `operator<` is some STL contains require their elements have defined this operator defined.

#### bool operator==(const FilePath& lhs, const FilePath& rhs)
#### bool operator!=(const FilePath& lhs, const FilePath& rhs)
#### bool operator<(const FilePath& lhs, const FilePath& rhs)

```c++
FilePath path1(L"c:\\abc\\def\\ghi.ado");
FilePath path2(L"C:\\ABC\\DEF\\GHI.ado");
FilePath path3(L"C:\\test\\unk");

path1 == path2;	// true
path1 != path3; // true
path1 < path3;	// true
```

The following are same as above operator, but are specific to `PathString`.

#### int CompareIgnoreCase(const PathString& str1, const PathString& str2) [*static*]
#### bool CompareEqualIgnoreCase(const PathString& str1, const PathString& str2) [*static*]

### Related to Internal String Value

---

#### const PathString& value() [*const*]

Returns a **const-reference** to the internal path-string value.

#### bool empty() [*const*]

Returns true, if the path is empty.

#### void clear() [*const*]

Empties the path object.

### Path Processnig

---

#### bool IsSeparator(PathChar ch) [*static*]

Returns `true`, if `ch` is a path separator. i.e. `\` or `/`;

Returns `false`, otherwise.

#### bool EndsWithSeparator() [*const*]

Returns `true`, if the path ends with a path separator.

Returns `false` otherwise.

#### FilePath AsEndingWithSeparator() [*const*]

Returns a copy of the file path that ends with a path separator.

If the path is empty, returns an empty `FilePath`.

#### FilePath StripTrailingSeparators() [*const*]

Returns a `FilePath` object that gets rid of the ending-path-separator.

If path doesn't end with path separator, the returned path is same as the origin.

#### FilePath NormalizePathSeparator() [*const*]

Returns a path that has its path separator normalized.

We choose `\\` as default path separator.

```c++
FilePath path(L"C:/test/path");
path = path.NormalizePathSeparator();	// now is C:\\test\\path
```

#### FilePath NormalizePathSeparatorTo(PathChar separator) [*const*]

Returns a path that has its path separator normalized to `separator` character.

```c++
FilePath path(L"C:\\test\\path");
// now is C:/test/path
path = path.NormalizePathSeparator(L'/');
```

#### FilePath DirName() [*const*]

Returns a FilePath corresponding to the dir that contains the path. If this object contains only one component, returns a FilePath identifying the current dir. If this object already refers to the root dir, returns a FilePath identifying the root dir.

```c++
const FilePath kCurrentPath(L".");
const FilePath kRootDir(L"..");

FilePath cur_paths[] { FilePath(L""), FilePath(L"."), FilePath(L"abc"), FilePath(L"./abc") };
for (const auto& path : cur_paths) {
    EXPECT_EQ(kCurrentPath, path.DirName());
}

FilePath root_paths[] { FilePath(L"../abc") };
for (const auto& path : root_paths) {
    EXPECT_EQ(kRootDir, path.DirName());
}

FilePath path(L"C:\\test\\path\\data.txt");
EXPECT_EQ(path.DirName(), FilePath(L"C:\\test\\path"));
```