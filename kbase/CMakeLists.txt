
add_library(kbase STATIC)

target_sources(kbase
  PRIVATE
    at_exit_manager.cpp
    at_exit_manager.h
    auto_reset.h
    base_path_provider.h
    base64.cpp
    base64.h
    basic_macros.h
    basic_types.h
    chrono_util.cpp
    chrono_util.h
    command_line.cpp
    command_line.h
    debugger.h
    environment.h
    error_exception_util.cpp
    error_exception_util.h
    file_info.h
    file_iterator.cpp
    file_iterator.h
    file_util.cpp
    file_util.h
    guid.cpp
    guid.h
    lazy.h
    logging.cpp
    logging.h
    lru_cache.h
    md5.cpp
    md5.h
    os_info.cpp
    os_info.h
    path_service.cpp
    path_service.h
    path.cpp
    path.h
    pickle.cpp
    pickle.h
    scope_guard.h
    scoped_handle.h
    secure_c_runtime.h
    signals.h
    singleton.h
    stack_walker.h
    string_encoding_conversions.cpp
    string_encoding_conversions.h
    string_format.cpp
    string_format.h
    string_util.cpp
    string_util.h
    string_view.h
    tokenizer.h

    $<$<BOOL:${WIN32}>:
      base_path_provider_win.cpp
      debugger_win.cpp
      environment_win.cpp
      file_iterator_win.cpp
      file_util_win.cpp
      file_version_info.cpp
      file_version_info.h
      minidump.cpp
      minidump.h
      os_info_win.cpp
      registry.cpp
      registry.h
      stack_walker_win.cpp
    >

    $<$<NOT:$<BOOL:${WIN32}>>:
      base_path_provider_posix.cpp
      debugger_posix.cpp
      environment_posix.cpp
      file_iterator_posix.cpp
      file_util_posix.cpp
      handle_interruptible_system_call.h
      os_info_posix.cpp
      stack_walker_posix.cpp
    >
)

target_include_directories(kbase
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../
)

target_link_libraries(kbase
  INTERFACE
    $<$<BOOL:${WIN32}>:Shlwapi>

    $<$<NOT:$<BOOL:${WIN32}>>:pthread>
)

apply_kbase_compile_conf(kbase)

if(MSVC AND KBASE_ENABLE_CODE_ANALYSIS)
  enable_kbase_msvc_static_analysis_conf(kbase
    WDL
      /wd6011 # Dereferencing potentially NULL pointer.
  )
endif()

set_target_properties(kbase PROPERTIES
  COTIRE_CXX_PREFIX_HEADER_INIT "${KBASE_PCH_HEADER}"
)

cotire(kbase)

get_target_property(kbase_FILES kbase SOURCES)
source_group("kbase" FILES ${kbase_FILES})
