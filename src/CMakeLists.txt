cmake_minimum_required(VERSION 3.1)

project(KBase CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(.)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/../../")

set(SOURCES
    kbase/at_exit_manager.cpp
    kbase/base_path_provider_posix.cpp
    kbase/error_exception_util.cpp
    kbase/path.cpp
    kbase/path_service.cpp
    kbase/pickle.cpp
    kbase/stack_walker_posix.cpp
    kbase/string_encoding_conversions.cpp
    kbase/string_format.cpp
    kbase/string_util.cpp)

add_library(kbase STATIC ${SOURCES})
