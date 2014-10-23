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

#### FilePath DirName() [*const*]

Returns a FilePath corresponding to the dir that contains the path.

If this object contains only one component, returns a FilePath identifying the current dir.

If this object already refers to the root dir, returns a FilePath identifying the root dir.

#### FilePath BaseName() [*const*]

Returns a FilePath corresponding to the file component of the path.

If this object already refers to the root, returns a FilePath identifying the root dir.

#### void GetComponents(std::vector<PathString>\* components) [*const*]

Retrieves every components of the path, including the root slash.

```c++
FilePath path(L"C:\\foo\\bar");
std::vector<FilePath::PathString> components;
path.GetComponents(&components);	// ["C:", "\\", "foo", "bar"]
```

#### bool IsAbsolute() [*const*]

Returns true if it is a absolute path.

#### bool ReferenceParent() [*const*]

Returns `true` if the path has a component that is '..'.

#### bool IsParent(const FilePath& child) [*const*]

Returns `true`, if the path is the parent of the `child`.

Returns `false` otherwise.

**NOTE:** This function may make a wrong judgement if paths that contain both `.` and `..` are involved.

#### void Append(const PathString& components)

#### void Append(const FilePath& components)

#### FilePath AppendTo(const PathString& components) [*const*]

#### FilePath AppendTo(const FilePath& components) [*const*]

#### void AppendASCII(const std::string& components)

#### FilePath AppendASCIITo(const std::string& components) [*const*]

**NOTE:** `components` must be a relative path. Otherwise, functions will throw an exception.

#### bool AppendRelativePath(const FilePath& child, FilePath* path) [*const*]

If current path is parent of the `child`, appends to `path` the relative path to child, and returns true.

otherwise, returns false.

```
Example: current path: C:\user\kingsley chen\
         child path:   C:\user\kingsley chen\app data\test
         *path:        C:\user\kingsley chen\documents\
After the calling of this function, *path becomes
C:\user\kingsley chen\documents\app data\test
```

#### PathString Extension() [*const*]

Returns the extension of the path if there is any.

The extension starts with extension separator.

If there are multiple extensions, Windows only recognizes the last one.

#### void RemoveExtension()

Removes the extension of the path if there is any.

#### FilePath StripExtention() [*const*]

Same as above, but strips on a copy and leaves the original path intact.

#### FilePath InsertBeforeExtension(const PathString& suffix) [*const*]

Inserts |suffix| after the file name portion of the path, but before the extension.

Returns an empty FilePath if the BaseName() is `.` or `..`.

```
Example: path: c:\foo\bar\test.jpg suffix: (1) --> c:\foo\bar\test(1).jpg
```

#### FilePath AddExtension(const PathString& extension) [*const*]

Adds extension to the file name of the path.

If the file name of the path already has an extension, the `extension` will be the sole extension recognized by Windows.

Returns an empty FilePath if the BaseName() is `.` or `..`.

#### FilePath ReplaceExtension(const PathString& extension) [*const*]

Replaces the extension of the file name with `extension`.

If `extension` is empty or only contains separator, the extension of the file name is removed.

If the file name does not have an extension, then `extension` is added.

Returns an empty FilePath if the BaseName() is `.` or `..`.

#### bool MatchExtension(const PathString& extension) [*const*]

Returns `true`, if `extension` matches the extension of the file name.

#### std::string AsASCII() [*const*]

#### std::string AsUTF8() [*const*]

Returns the string in native encoding of the path.

If the path contains any non-ASCII character, the return value is an empty object.

#### static FilePath FromASCII(const std::string& path_in_ascii)

#### static FilePath FromUTF8(const std::string& path_in_utf8)

If the `path_in_ascii` contains any non-ASCII character, the function returns an empty FilePath.

### Serialization

---

#### void WriteToPickle(Pickle* pickle) [*const*]

#### bool ReadFromPickle(PickleIterator* iter)
