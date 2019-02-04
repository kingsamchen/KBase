
KBase
==========

## Introduction

A light-weight base library for C++.


## Build Instructions

### Windows

**Requirements**

- Windows 7, or later
- Visual Studio 2015, or later (C++ 14 is required)
- CMake 3.11 or later
- Python 3

Note:

- If python 3 was not installed, you should run cmake configuration and build targets manually.
- We will use latest version of Visual Studio as possible and use x64 as the default target platform.

**Build Steps**

1. Clone the repository and `cd` to the `KBase` directory
2. Run `python ./gen.py`
3. It will run cmake configuration and build `kbase` and its `tests` automatically.
4. By default, generated files are located in folder `build/Windows/{build-type}`.

### Ubuntu

**Requirements**

- 14.04 LTS x64, or later
- Clang 3.8, or G++ as the minimum (C++ 14 is required)
- CMake 3.11, or later
- Python 3
- Ninja (optional)

Note:
- If python 3 was not installed, you should run cmake configuration and build targets manually.
- If Ninja was not installed, the `gen.py` would automatically fallback to using makefile.

**Build Steps**

1. Clone the repository, and `cd` to `KBase` directory
2. Run `gen.py` to build the lib and tests.
   Use `--build-type={Debug|Release}` to specify build mode, and *Debug* is the default mode.
3. By default, generated files are located in `build/{build-type}`

Please be noted that, building the project would not install any of its files into your system's include directory.

### Misc

Run `gen.py --help` to check command flags in details.


## Usages and Samples

The project `Test` contains a set of unit test files, which can also be regarded as code samples in a certain of extend.

Documentation files in `docs` are far more outdated, and it will take a lot of work to make it consistent with newest version of the codebase.


## Disclaimers

KBase is initially a practicing project for fun, i.e. a personal-interest-driven project. There is no guarantee for its long-term maintenance.

KBase is inspired by base lib in Chromium. However, I don't approve of the way how they use C++, thus I build KBase in my own coding flavor, and also with adapting to my personal needs.
