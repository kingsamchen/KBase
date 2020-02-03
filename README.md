
KBase
==========

## Introduction

A light-weight base library for modern C++.



## Build Instructions

### Platform Requirements

#### Windows

- Windows 7, or later
- Visual Studio 2015, or later (C++ 14 is required)
- CMake 3.14 or later

Note:

- We will use the latest version of Visual Studio by default, and use x64 as the default target architecture.

#### Ubuntu

- 14.04 LTS x64, or later
- Clang 3.8, or G++ 5 as the minimum (C++ 14 is required)
- CMake 3.14 or later
- Ninja (optional)

Note:

- If Ninja was not found, the traditional Makefile would be used.

### Generate & Build

#### For Windows / MSVC

```
$ cd KBase
$ ./anvil.ps1 [-build-type <Debug|Release>] [-no-build]
```

If not specified, default build is in `Release` mode; and if `-no-build` is specified, generate Visual Studio solution project only.

#### For Linux

```
$ cd KBase
$ ./anvil.sh [--build-type=<Debug|Release>] [--no-build]
```

If not specified, default build is in `Release` mode; and if `-no-build` is specified, generate makefile or build.ninja files only.

You can force using generator by:

```
$ GENERATOR="Unix Makefiles" ./anvil.sh
```

Please be noted that, building the project on Linux platforms would not install any of its files into your system's include directory.

#### Addendum

Release build would contain debugging symbols, which is controlled by `compiler_*.cmake` files.

If you need to tailor build script to meet your needs, feel free to edit it.

## As Dependency of a CMake Project

KBase is _sub-project-aware_, which means you can include it into your CMake project by using `add_subdirectory()` command.

CMake's [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) module maybe a good start to automate populating dependencies.

Or just use [CPM](https://github.com/TheLartians/CPM.cmake) to simplify the entire process; KBase itself uses CPM to manages the Catch2 library.

Once complete integration, use `target_link_libraries()` command to declare dependency for your executable or library target.

```cmake
add_executable(my_exe main.cpp)
target_link_libraries(my_exe
  PRIVATE kbase
)
```

NOTE: Building & installing then using `find_package()` to introduce dependency of KBase is currently not supported.

## Usages and Samples

The project `Test` contains a set of unit test files, which can also be regarded as code samples in a certain of extend.

Documentation files in `docs` are far more outdated, and it will take a lot of work to make it consistent with newest version of the codebase.

## Disclaimers

KBase is initially a practicing project for fun, i.e. a personal-interest-driven project. There is no guarantee for its long-term maintenance.

KBase is inspired by base lib in Chromium. However, I don't approve of the way how they use C++, thus I build KBase in my own coding flavor, and also with adapting to my personal needs.
