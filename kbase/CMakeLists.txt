
set(kbase_SRCS
  at_exit_manager.cpp
  base64.cpp
  chrono_util.cpp
  command_line.cpp
  error_exception_util.cpp
  file_iterator.cpp
  file_util.cpp
  guid.cpp
  logging.cpp
  md5.cpp
  os_info.cpp
  path.cpp
  path_service.cpp
  pickle.cpp
  string_encoding_conversions.cpp
  string_format.cpp
  string_util.cpp
)

if(WIN32)
  list(APPEND kbase_SRCS
    base_path_provider_win.cpp
    debugger_win.cpp
    environment_win.cpp
    file_iterator_win.cpp
    file_util_win.cpp
    file_version_info.cpp
    minidump.cpp
    os_info_win.cpp
    registry.cpp
    stack_walker_win.cpp
  )
elseif(UNIX)
  list(APPEND kbase_SRCS
    base_path_provider_posix.cpp
    debugger_posix.cpp
    environment_posix.cpp
    file_iterator_posix.cpp
    file_util_posix.cpp
    os_info_posix.cpp
    stack_walker_posix.cpp
  )
endif()

set(kbase_HEADERS
  at_exit_manager.h
  auto_reset.h
  base_path_provider.h
  base64.h
  basic_macros.h
  basic_types.h
  chrono_util.h
  command_line.h
  debugger.h
  environment.h
  error_exception_util.h
  file_info.h
  file_iterator.h
  file_util.h
  guid.h
  lazy.h
  logging.h
  lru_cache.h
  md5.h
  os_info.h
  path.h
  path_service.h
  pickle.h
  scope_guard.h
  scoped_handle.h
  secure_c_runtime.h
  signals.h
  singleton.h
  stack_walker.h
  string_encoding_conversions.h
  string_format.h
  string_util.h
  string_view.h
  tokenizer.h
)

if(WIN32)
  list(APPEND kbase_HEADERS
    file_version_info.h
    minidump.h
    registry.h
  )
elseif(UNIX)
  list(APPEND kbase_HEADERS
    handle_interruptible_system_call.h
  )
endif()

set(kbase_FILES ${kbase_HEADERS} ${kbase_SRCS})

source_group("kbase" FILES ${kbase_FILES})

add_library(kbase ${kbase_FILES})

apply_common_compile_properties_to_target(kbase)

if(MSVC)
  enable_msvc_static_analysis_for_target(kbase)
endif()

target_include_directories(kbase
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../
)

set_target_properties(kbase PROPERTIES
  COTIRE_CXX_PREFIX_HEADER_INIT "${KBASE_PCH_HEADER}"
)

cotire(kbase)

if(WIN32)
  target_link_libraries(kbase INTERFACE Shlwapi)
else(UNIX)
  target_link_libraries(kbase INTERFACE pthread)
endif()
