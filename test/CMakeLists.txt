cmake_minimum_required(VERSION 2.8)

project(Test CXX)

set(CXX_FLAGS
    -g
    -rdynamic)

include_directories("third-party/gtest/include" "../src")

set(PROJECT_LINK_LIBS "libkbase.a" "libgtest.a" "pthread")
link_directories(${CMAKE_BINARY_DIR}/../../)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../../")
set(SOURCES
    main.cpp
    samples/stack_walker_unittest.cpp
    )

add_executable(test ${SOURCES})
target_link_libraries(test ${PROJECT_LINK_LIBS})
