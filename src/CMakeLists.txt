set(zinhart_lib 
  zinhart.cc
   )	
 add_library(zinhart ${zinhart_lib} )
 
 #link cpu threading library
 target_link_libraries(
 zinhart 
 ${CMAKE_THREAD_LIBS_INIT}
)

if(BuildShared)
  #install  libzinhart.a to <cmake output dir>/${CMAKE_CURRENT_DIR}/
  install(TARGETS zinhart LIBRARY DESTINATION lib LIBRARY DESTINATION lib)
else()
  # install  libzinhart.a to <cmake output dir>/${CMAKE_CURRENT_DIR}/
  install(TARGETS zinhart LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
endif()

#to do
install(DIRECTORY ../include/zinhart DESTINATION include)
