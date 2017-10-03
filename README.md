
KBase
==========

## Introduction

A light-weight base library for C++.


## Build Instructions

### Windows

**Requirements**

- Windows 7, or later
- Visual Studio 2015, or later

**Build Steps**

1. Clone the repository
2. Open the `KBase.sln` file, then build the project `KBase`, it will end up with the static library `kbase.lib` in the directory `KBase/build/<Configuration>/`
3. Build the `Test` project if you want to run any unittests (optional).

Please be noted that, only x64 mode compilation is officially supported, though compiling on 32-bit should also work.

Besides, all these projects originally are built with `/MD(d)` flags.

### Ubuntu

**Requirements**

- 14.04 LTS x64, or later
- Clang 3.8, or G++ 6 (higher versions should also work)
- CMake 3.1, or later
- Python 2.7+

Note: G++ 5 has bugs which cause compilation errors.

**Build Steps**

1. Clone the repository, and `cd` to `KBase` directory
2. Execute `gen_kbase.py` to build the project `KBase`, it will end up with the static library `libkbase.a` in the directory `KBase/build/<BuildType>/`
   use `--build-type={Debug|Release}` to specify build mode, and *Debug* is the default mode.
3. If you want to build the `Test` project, run `gen_test.py`
   switch `--build-type` is also available here; moreover, if you want to build the `Test` project altogether with `KBase`, run `gen_test.py` with switch `--build-all`.

Please be noted that, building `KBase` project would not install any of its files into system's include directory.


## Usages and Samples

The project `Test` contains a set of unit test files, which can also be regarded as code samples in a certain of extend.

Documentation files in `docs` are far more outdated, and it will take a lot of work to make it consistent with newest version of the codebase.


## Disclaimers

KBase is initially a practicing project for fun, i.e. a personal-interest-driven project. There is no guarantee for its long-term maintenance.

KBase is inspired by base lib in Chromium. However, I don't approve of the way how they use C++, thus I build KBase in my own coding flavor, and also with adapting to my personal needs.
