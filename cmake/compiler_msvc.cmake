
option(MSVC_ENABLE_PARALLEL_BUILD "If enabled, build multiple files in parallel." ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "limited configs" FORCE)

function(apply_kbase_compile_conf TARGET)
  target_compile_definitions(${TARGET}
    PUBLIC
      _UNICODE
      UNICODE
      NOMINMAX

      $<$<CONFIG:DEBUG>:
        _DEBUG
      >
  )

  target_compile_options(${TARGET}
    PRIVATE
      /W4
      /WX # Treat warning as error.
      /wd4819 # source characters not in current code page.

      /Zc:inline # Have the compiler eliminate unreferenced COMDAT functions and data before emitting the object file.

      $<$<BOOL:${MSVC_ENABLE_PARALLEL_BUILD}>:/MP>

    PUBLIC
      /Zc:referenceBinding # Disallow temporaries from binding to non-const lvalue references.
      /Zc:rvalueCast # Enforce the standard rules for explicit type conversion.
      /Zc:implicitNoexcept # Enable implicit noexcept specifications where required, such as destructors.
      /Zc:strictStrings # Don't allow conversion from a string literal to mutable characters.
      /Zc:threadSafeInit # Enable thread-safe function-local statics initialization.
      /Zc:throwingNew # Assume operator new throws on failure.

      /permissive- # Be mean, don't allow bad non-standard stuff (C++/CLI, __declspec, etc. are all left intact).
  )
endfunction()

# Enable static analysis for all targets could result in excessive warnings.
# Thus we decided to enable it for targets only we explicitly specify.
# Use /wd args to suppress analysis warnings we cannot resolve.
function(enable_kbase_msvc_static_analysis_conf TARGET)
  set(multiValueArgs WDL)
  cmake_parse_arguments(ARG "" "" "${multiValueArgs}" ${ARGN})

  target_compile_options(${TARGET}
    PRIVATE
      /analyze
      /analyze:WX-

      ${ARG_WDL}
  )
endfunction()
