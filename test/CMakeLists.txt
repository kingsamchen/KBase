
set(test_SRCS
    main.cpp
    tests/at_exit_manager_unittest.cpp
    tests/auto_reset_unittest.cpp
    tests/base64_unittest.cpp
    tests/chrono_util_unittest.cpp
    tests/command_line_unittest.cpp
    tests/debugger_unittest.cpp
    tests/enum_ops_unittest.cpp
    tests/environment_unittest.cpp
    tests/error_exception_util_unittest.cpp
    tests/file_iterator_unittest.cpp
    tests/file_util_unittest.cpp
    tests/guid_unittest.cpp
    tests/lazy_unittest.cpp
    tests/logging_unittest.cpp
    tests/lru_cache_unittest.cpp
    tests/md5_unittest.cpp
    tests/os_info_unittest.cpp
    tests/path_service_unittest.cpp
    tests/path_unittest.cpp
    tests/pickle_unittest.cpp
    tests/scope_guard_unittest.cpp
    tests/signals_unittest.cpp
    tests/singleton_unittest.cpp
    tests/stack_walker_unittest.cpp
    tests/string_encoding_conversions_unittest.cpp
    tests/string_format_unittest.cpp
    tests/string_util_unittest.cpp
    tests/string_view_unittest.cpp
    tests/tokenizer_unittest.cpp
)

add_executable(kbase_test ${test_SRCS})

target_include_directories(kbase_test
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Catch2/single_include
)

target_link_libraries(kbase_test PUBLIC kbase pthread)
