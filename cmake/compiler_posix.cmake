
set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")

function(apply_kbase_compile_conf TARGET)
  target_compile_definitions(${TARGET}
    PUBLIC
      $<$<CONFIG:DEBUG>:
        _DEBUG
      >
  )

  target_compile_options(${TARGET}
    PRIVATE
      -g
      -Wall
      -Wextra
      -Werror
      -Wno-unused-parameter
      -Wold-style-cast
      -Woverloaded-virtual
      -Wpointer-arith
      -Wshadow

      $<$<STREQUAL:${CMAKE_CXX_COMPILER_ID},"Clang">:-fno-limit-debug-info>
  )
endfunction()
