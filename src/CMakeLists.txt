cmake_minimum_required(VERSION 2.8)

project(KBase CXX)

include_directories(.)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/../../")

set(SOURCES 
    kbase/stack_walker_posix.cpp)

add_library(kbase STATIC ${SOURCES})
