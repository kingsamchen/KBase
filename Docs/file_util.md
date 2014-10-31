File Util
===

`file_util` provides a bunch of handy utilities for manipulating files and directories.

---

#### FilePath MakeAbsoluteFilePath(const FilePath& path)

Returns an absolute or full path name of the relative |path|.

Returns an empty path on error.

#### bool PathExists(const FilePath& path)

Returns true if the path exists.

Returns false if doesn't exist.

#### bool DirectoryExists(const FilePath& path)

Returns true if the directory exists.

Returns false, otherwise.

#### bool IsDirectoryEmpty(const FilePath& path)

Returns true if the directory is empty.

Returns false otherwise.

#### FileInfo GetFileInfo(const FilePath& path)

Retrieves the information of a given file or directory.

Throws an exception when error occurs.

Be wary of that the size-field of the `FileInfo` is valid if and only if the `path` indicates a file.

#### void RemoveFile(const FilePath& path, bool recursive)

Removes a file or a directory indicated by the given |path|.

If want to remove a directory non-recursively, the directory must be empty.

Throws an exception when failed.

#### void RemoveFileAfterReboot(const FilePath& path)

If the `path` refers to a directory, the system removes the directory only if the directory is empty.

This function only marks the file or directory as should-be-deleted, it doesn't guarantee the deletion will be enforced.

Throws an exception when failed to mark.

#### void DuplicateFile(const FilePath& src, const FilePath& dest)

Copies a single file from |src| to |dest|.

If `dest` already exists, has it overwritten.

The file copied retains file attribute from the source.

#### void DuplicateDirectory(const FilePath& src, const FilePath& dest, bool recursive)

Copies all files in `src` to `dest` if recursive is false.

Copies all content, including subfolders in `src` to `dest` if recursive is true.

Overwrites files that already exist.

#### void MakeFileMove(const FilePath& src, const FilePath& dest)

Moves a file or a directory along with its subfolders from `src` to `dest`.

Overwrites any that already exists.

#### std::string ReadFileToString(const FilePath& path);
#### void ReadFileToString(const FilePath& path, std::string* data);

Reads contents of whole file at `path` into a string.

Data is read in binary mode, therefore no CRLF conversion involved.

If failed to read from file, the string is empty.

#### void WriteStringToFile(const FilePath& path, const std::string& data)

Writes `data` to a file at `path`.

Be wary of that in this function, the data is written in text-mode.

If failed to create/open the file to write, the function does nothing.

#### void AppendStringToFile(const FilePath& path, const std::string& data)

Similar to WriteStringToFile, but instead of overwritting the existing contents, this function has data appended.

If failed to create/open the file to write, the function does nothing.