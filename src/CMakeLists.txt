cmake_minimum_required(VERSION 2.8)

project(KBase CXX)

include_directories(.)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/../../")

set(SOURCES
    kbase/at_exit_manager.cpp
    kbase/error_exception_util.cpp
    kbase/path.cpp
    kbase/stack_walker_posix.cpp
    kbase/string_encoding_conversions.cpp
    kbase/string_format.cpp
    kbase/string_util.cpp)

add_library(kbase STATIC ${SOURCES})
