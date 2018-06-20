add_library(gtest_zinhart ./gtest/gtest-all.cc)
include_directories(.)
target_link_libraries(gtest_zinhart ${CMAKE_THREAD_LIBS_INIT})

set(test_src
  run_all.cc
  binary_parser_test.cc
  )

add_executable(zinhart_tests ${test_src})

target_link_libraries(zinhart_tests
  gtest_zinhart
  zinhart
  ${CMAKE_THREAD_LIBS_INIT}
  ${MATH_LIBS}
)

add_test(
  NAME tests
  COMMAND zinhart_tests
)
