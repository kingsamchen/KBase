KBase
==========

### Introduction

A light-weight base library for C++.

### Compatibility

KBase is initially a toy project for fun, not aimd for cross-platform.

Since I made these files by using Visual Studio 2013 on Windows 7, some facilities or infrastructures that do jeopardize library portability are inevitably employed.

- part of C++11 features
- brand new APIs provided only since Windows Vista/7
- un-portable CRT implementations, such as secured-string-manipulation functions 

Therefore, the **minimum** requirements are

- Visual Studio 2013 on Windows 7 (32/64-bit)
- GNU C++ that supports C++11 on cygwin might possibly be used if non-portable part was replaced with standard counterparts or your own implementations.


### Installation

KBase aims to be a static library. Just download the source code and open the `.sln` file with your VS then build, ending up with `kbase.lib`。

If you use GNU C++, you may need to write makefile to build the entire project on your own.

### Disclaimer

Once again, KBase is a toy project I do in spare time. There is no any guarantee for its long-term maintenance.

KBase is inspired by base of Chromium. However, I don't approve of the way they use C++, thus I wrote KBase in my own coding flavor, and also with adapting to my personal needs.

### Usage and Samples

Directory `Docs` contains documentations for explaining how to use each part of KBase.

The sub-directory `KBase_Test` contains a series of unit test files, which can be regarded as code samples in a certain of extend.

The unit test framework I use is Google GTest.

Though archives in these two directories may not as detailed as they should be, I will do my best to make them complete.