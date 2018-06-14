
set(test_SRCS
    main.cpp
    samples/at_exit_manager_unittest.cpp
    samples/auto_reset_unittest.cpp
    samples/base64_unittest.cpp
    samples/command_line_unittest.cpp
    samples/chrono_util_unittest.cpp
    samples/debugger_unittest.cpp
    samples/enum_ops_unittest.cpp
    samples/environment_unittest.cpp
    samples/error_exception_util_unittest.cpp
    samples/file_iterator_unittest.cpp
    samples/file_util_unittest.cpp
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

add_executable(kbase_test ${test_SRCS})

target_link_libraries(kbase_test PUBLIC kbase gtest pthread)
