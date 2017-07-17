cmake_minimum_required(VERSION 3.1)

project(Test CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CXX_FLAGS
    -g
    -rdynamic)

include_directories("third-party/gtest/include" "../src")

set(PROJECT_LINK_LIBS "libkbase.a" "libgtest.a" "pthread")
link_directories(${CMAKE_BINARY_DIR}/../)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../")
set(SOURCES
    main.cpp
    samples/at_exit_manager_unittest.cpp
    samples/auto_reset_unittest.cpp
    samples/error_exception_util_unittest.cpp
    samples/guid_unittest.cpp
    samples/lazy_unittest.cpp
    samples/logging_unittest.cpp
    samples/lru_cache_unittest.cpp
    samples/md5_unittest.cpp
    samples/os_info_unittest.cpp
    samples/path_service_unittest.cpp
    samples/path_unittest.cpp
    samples/pickle_unittest.cpp
    samples/scope_guard_unittest.cpp
    samples/signals_unittest.cpp
    samples/singleton_unittest.cpp
    samples/stack_walker_unittest.cpp
    samples/string_encoding_conversions_unittest.cpp
    samples/string_format_unittest.cpp
    samples/string_util_unittest.cpp
    samples/string_view_unittest.cpp
    samples/tokenizer_unittest.cpp
    )

add_executable(kbase_test ${SOURCES})
target_link_libraries(kbase_test ${PROJECT_LINK_LIBS})
