
set(CATCH2_SOURCE_DIR ${DEPS_SOURCE_DIR}/catch2-src)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
  GIT_TAG         v2.5.0
  GIT_SHALLOW     TRUE
  SOURCE_DIR      ${CATCH2_SOURCE_DIR}
)

FetchContent_GetProperties(Catch2)

if(NOT catch2_POPULATED)
  if(EXISTS "${CATCH2_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "Catch2 source dir is already ready; skip downloading.")
    set(FETCHCONTENT_SOURCE_DIR_CATCH2 ${CATCH2_SOURCE_DIR})
  endif()

  FetchContent_Populate(Catch2)
  message(STATUS "Catch2 source dir: ${catch2_SOURCE_DIR}")
  message(STATUS "Catch2 binary dir: ${catch2_BINARY_DIR}")
  add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
endif()
