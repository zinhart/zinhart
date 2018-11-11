# cpu & gpu unit tests
if(BuildCuda)
  #unit test src
  set(concurrent_routines_unit_tests_src
   run_all.cc
   thread_pool_test.cc
   # cpu_test.cc
   gpu_test.cc 
   )
add_executable(concurrent_routines_unit_tests ${concurrent_routines_unit_tests_src})

target_link_libraries(concurrent_routines_unit_tests 
					  gtest_main
					  concurrent_routines
					  ${CMAKE_THREAD_LIBS_INIT}
					  )
add_test(
  NAME concurrent_routines_unit_tests
  COMMAND concurrent_routines_unit_tests
  )
# cpu only unit tests
else()
  # multi_core unit test src
  set(zinhart_unit_tests_src
   run_all.cc
   bmp_parser_test.cc
   )
add_executable(zinhart_unit_tests ${zinhart_unit_tests_src})

target_link_libraries(zinhart_unit_tests 
					  gtest
					  #gmock_main # for google mock
					  multi_core
					  ${CMAKE_THREAD_LIBS_INIT}
					  )
add_test(
  NAME zinhart_unit_tests
  COMMAND zinhart_unit_tests
  )
endif()

