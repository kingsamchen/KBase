
declare_dep_module(Catch2
  VERSION         2.5.0
  GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
  GIT_TAG         v2.5.0
)

set(tests_SRCS
  main.cpp
  at_exit_manager_unittest.cpp
  auto_reset_unittest.cpp
  base64_unittest.cpp
  chrono_util_unittest.cpp
  command_line_unittest.cpp
  debugger_unittest.cpp
  enum_ops_unittest.cpp
  environment_unittest.cpp
  error_exception_util_unittest.cpp
  file_iterator_unittest.cpp
  file_util_unittest.cpp
  guid_unittest.cpp
  lazy_unittest.cpp
  logging_unittest.cpp
  lru_cache_unittest.cpp
  md5_unittest.cpp
  os_info_unittest.cpp
  path_service_unittest.cpp
  path_unittest.cpp
  pickle_unittest.cpp
  scope_guard_unittest.cpp
  scoped_handle_unittest.cpp
  signals_unittest.cpp
  singleton_unittest.cpp
  stack_walker_unittest.cpp
  string_encoding_conversions_unittest.cpp
  string_format_unittest.cpp
  string_util_unittest.cpp
  string_view_unittest.cpp
  tokenizer_unittest.cpp
)

if(WIN32)
  list(APPEND tests_SRCS
    file_version_info_unittest.cpp
    minidump_unittest.cpp
    registry_unittest.cpp
  )
elseif(UNIX)
  list(APPEND tests_SRCS
  )
endif()

source_group("tests" FILES ${tests_SRCS})

add_executable(kbase_test ${tests_SRCS})

apply_common_compile_properties_to_target(kbase_test)

set_target_properties(kbase_test PROPERTIES
  COTIRE_CXX_PREFIX_HEADER_INIT "${KBASE_PCH_HEADER}"
)

cotire(kbase_test)

target_link_libraries(kbase_test PRIVATE kbase Catch2)
