cmake_minimum_required(VERSION 3.1)

project(gtest CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CXX_FLAGS
    -g
    -rdynamic)

include_directories(gtest/include gtest)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/../../")
file(GLOB SOURCES "gtest/src/gtest-all.cc")

add_library(gtest STATIC ${SOURCES})