
KBase
==========

### Introduction

A light-weight base library for C++.

### Build Instructions

**Requirements**

- Windows 7, or later
- Visual Studio 2015, or later

Neither other platforms, nor other compilers are officially supported; although, compilers that support C++ 14 features and support building Windows native code, should work.

Cross-platform is planned, but not yet scheduled. And since quite a few Windows-specific APIs are (inevitably) used, it would take time to make the code work on other platforms.

I will update the readme file as soon as the port progress is made.

**Build Steps**

1. Clone the repository
2. Open the `KBase.sln` file, then build the project `KBase`, it will end up with the static library `kbase.lib` in the directory `KBase/build/<Configuration>/`
3. If you want to build the `Test` project, then
    1. first need to build `gtest` project, for which just open solution file `KBase/test/third-party/gtest.sln`, then build the project.
    2. build `Test` project

Please be noted that, only 64-bit code compilation is officially supported, though 32-bit should work too.

Besides, all these projects originally are build with `/MD(d)` flags.

### Usages and Samples

Directory `docs` contains documentations for explaining how to use each facility of KBase.

The sub-project `Test` contains a set of unit test files, which can also be regarded as code samples in a certain of extend.

Though archives in these two directories may not being detailed as they should be, I will do my best to make them complete.

### Disclaimers

KBase is initially a practicing project for fun, i.e. a personal-interest-driven project. There is no guarantee for its long-term maintenance.

KBase is inspired by base lib from Chromium. However, I don't approve of the way how they use C++, thus I build KBase in my own coding flavor, and also with adapting to my personal needs.