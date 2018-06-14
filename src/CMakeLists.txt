
set(kbase_SRCS
    kbase/at_exit_manager.cpp
    kbase/base_path_provider_posix.cpp
    kbase/base64.cpp
    kbase/command_line.cpp
    kbase/chrono_util.cpp
    kbase/debugger_posix.cpp
    kbase/environment_posix.cpp
    kbase/error_exception_util.cpp
    kbase/file_iterator.cpp
    kbase/file_iterator_posix.cpp
    kbase/file_util.cpp
    kbase/file_util_posix.cpp
    kbase/guid.cpp
    kbase/logging.cpp
    kbase/md5.cpp
    kbase/os_info.cpp
    kbase/os_info_posix.cpp
    kbase/path.cpp
    kbase/path_service.cpp
    kbase/pickle.cpp
    kbase/stack_walker_posix.cpp
    kbase/string_encoding_conversions.cpp
    kbase/string_format.cpp
    kbase/string_util.cpp
)

add_library(kbase ${kbase_SRCS})

target_include_directories(kbase
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
