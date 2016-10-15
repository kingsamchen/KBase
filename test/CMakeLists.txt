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
    samples/at_exit_manager_unittest.cpp
    samples/error_exception_util_unittest.cpp
    samples/path_unittest.cpp
    samples/pickle_unittest.cpp
    samples/stack_walker_unittest.cpp
    samples/string_encoding_conversions_unittest.cpp
    samples/string_format_unittest.cpp
    samples/string_util_unittest.cpp
    samples/string_view_unittest.cpp
    samples/tokenizer_unittest.cpp
    )

add_executable(test ${SOURCES})
target_link_libraries(test ${PROJECT_LINK_LIBS})
