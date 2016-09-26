cmake_minimum_required(VERSION 2.8)

project(gtest)

include_directories(gtest/include gtest)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/../../")
file(GLOB SOURCES "gtest/src/gtest-all.cc")

add_library(gtest STATIC ${SOURCES})