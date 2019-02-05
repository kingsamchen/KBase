
set(CATCH2_SOURCE_NAME Catch2)
set(CATCH2_SOURCE_VER v2.5.0)

# Content name is used by the module and in the lowercase.
string(TOLOWER "${CATCH2_SOURCE_NAME}-${CATCH2_SOURCE_VER}" CATCH2_CONTENT_NAME)

# Source dir is where we put the source code of the dependency.
set(CATCH2_SOURCE_DIR "${DEPS_SOURCE_DIR}/${CATCH2_CONTENT_NAME}-src")

FetchContent_Declare(
  ${CATCH2_CONTENT_NAME}
  GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
  GIT_TAG         ${CATCH2_SOURCE_VER}
  GIT_SHALLOW     TRUE
  SOURCE_DIR      ${CATCH2_SOURCE_DIR}
)

FetchContent_GetProperties(${CATCH2_CONTENT_NAME})

if(NOT ${CATCH2_CONTENT_NAME}_POPULATED)
  message(STATUS "Fetching dep: ${CATCH2_CONTENT_NAME}")

  if(EXISTS "${CATCH2_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "${CATCH2_CONTENT_NAME} source dir is already ready; skip downloading.")
    set(FETCHCONTENT_SOURCE_DIR_CATCH2 ${CATCH2_SOURCE_DIR})
  endif()

  FetchContent_Populate(${CATCH2_CONTENT_NAME})

  # Set two module-defined variables.
  set(CATCH2_CONTENT_SOURCE_DIR_VAR "${${CATCH2_CONTENT_NAME}_SOURCE_DIR}")
  set(CATCH2_CONTENT_BINARY_DIR_VAR "${${CATCH2_CONTENT_NAME}_BINARY_DIR}")

  message(STATUS "${CATCH2_CONTENT_NAME} source dir: ${CATCH2_CONTENT_SOURCE_DIR_VAR}")
  message(STATUS "${CATCH2_CONTENT_NAME} binary dir: ${CATCH2_CONTENT_BINARY_DIR_VAR}")

  add_subdirectory(${CATCH2_CONTENT_SOURCE_DIR_VAR} ${CATCH2_CONTENT_BINARY_DIR_VAR})
endif()
