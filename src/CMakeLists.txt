if(BuildCuda)
  set(multi_core_lib
	  multi_core.cu
	  multi_core.cc
	  multi_core_error.cc
	  serial/serial.cc
	  #parallel/parallel.cc
	  parallel/thread_pool.cc
  )
  # link with cuda lib
  cuda_add_library(multi_core ${LIB_TYPE} ${multi_core_lib})
  
  #link cpu threading library
  target_link_libraries(
   multi_core 
   ${CUDA_CUBLAS_LIBRARIES}
   ${CMAKE_THREAD_LIBS_INIT}
  )

  if(BuildShared)
	#install  libmulti_core.so to <cmake output dir>/${CMAKE_CURRENT_DIR}/
	install(TARGETS multi_core LIBRARY DESTINATION lib LIBRARY DESTINATION lib)
  else()
	# install  libmulti_core.a to <cmake output dir>/${CMAKE_CURRENT_DIR}/
	install(TARGETS multi_core LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
  endif()

  #to do
  install(DIRECTORY ../include/zinhart DESTINATION include)
else()
  set(zinhart_lib 
	  zinhart.cc
     )	
   add_library(zinhart ${LIB_TYPE} ${zinhart_lib})
   
   #link cpu threading library & MKL
   target_link_libraries(zinhart
   multi_core 
   bmp
   ann
   #   ${MKL_LIBRARIES}
   ${CMAKE_THREAD_LIBS_INIT}
  )

  if(BuildShared)
	#install  libzinhart.so to <cmake output dir>/${CMAKE_CURRENT_DIR}/
	install(TARGETS zinhart LIBRARY DESTINATION lib DESTINATION lib)
  else()
	# install  libzinhart.a to <cmake output dir>/${CMAKE_CURRENT_DIR}/
	install(TARGETS zinhart LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
  endif()

  #to do
  install(DIRECTORY ../include/zinhart DESTINATION include)
endif()


