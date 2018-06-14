
set(gtest_SRCS
    gtest/src/gtest-all.cc
)

add_library(gtest ${gtest_SRCS})

target_include_directories(gtest
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/gtest/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/gtest
)
