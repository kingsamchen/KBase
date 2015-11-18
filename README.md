KBase
==========

### Introduction

A light-weight base library for C++.

### Compatibility

KBase is initially a toy project for fun, not aimd for cross-platform.

Since I build this project with Visual Studio 2015 on Windows 10, some facilities or infrastructures that may jeopardize code portability to other compilers or platforms are inevitably employed.

Including but not limited to

- part of C++11 features
- brand new APIs firstly introduced since Windows Vista/7
- un-portable CRT implementations, such as secured-string-manipulation functions 

Therefore, the **minimum** requirements are

- Visual Studio 2013 on Windows 7 (32/64-bit, thought X64 is the default target platform)
- GNU C++ that supports C++11 on cygwin could possibly be used if non-portable part was replaced with standard counterparts or your own implementations.

### Compilation

KBase aims to be a static library. Just download the source code and open the `.sln` file with your VS then build, ending up with `kbase.lib`。

If you use GNU C++, you may need to write makefile on your own to build the entire project.

The test sub-project is built on top of GTest, please have it built in advance. The details of how `gtest.lib` is referenced can be examined from project setting files.

### Disclaimer

Once again, KBase is a personal-interest-driven project. There is no any guarantee for its long-term maintenance.

KBase is inspired by base lib of Chromium. However, I don't approve of the way they use C++, thus I build KBase in my own coding flavor, and also with adapting to my personal needs.

### Usage and Samples

Directory `Docs` contains documentations for explaining how to use each part of KBase.

The sub-directory `KBase_Test` contains a series of unit test files, which can be regarded as code samples in a certain of extend.

Though archives in these two directories may not as detailed as they should be, I will do my best to make them complete.