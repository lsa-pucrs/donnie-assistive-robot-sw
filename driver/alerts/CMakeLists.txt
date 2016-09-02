CMAKE_MINIMUM_REQUIRED (VERSION 2.4 FATAL_ERROR)
PROJECT (alerts)

# Include this CMake module to get most of the settings needed to build
SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")
INCLUDE (UsePlayerPlugin) # defines PLAYER_ADD_PLUGIN_DRIVER
INCLUDE (CheckCXXCompilerFlag) # defines CHECK_CXX_COMPILER_FLAG

# cFUGUE requires gcc 4.8. It requires gnu++11
IF(CMAKE_COMPILER_IS_GNUCC)
  #SET(TARGET_COMPILE_FLAGS "-std=gnu++0x") # Add C++11 support for GCC
  #if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
  #    message(FATAL_ERROR "GCC version must be at least 4.8!")
  #endif()  
  execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)
  if (GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
     message(STATUS "Version >= 4.8")
  else ()
	 message(FATAL_ERROR "GCC version must be at least 4.8!")
  endif() 

  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  if(COMPILER_SUPPORTS_CXX11)
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
     message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
  endif()
  # -std=gnu++11 is only avaible in version 4.7 or latter
  #SET(TARGET_COMPILE_FLAGS "-std=gnu++11") # Add C++11 support for GCC
ENDIF()

SET (CFUGUE_INCLUDE_DIR /home/lsa/Desktop/tone-generator-test/CFugue/include
						/home/lsa/Desktop/tone-generator-test/CFugue/include/common 
						/home/lsa/Desktop/tone-generator-test/CFugue/test/../include 
						/home/lsa/Desktop/tone-generator-test/CFugue/test/../include/Common 
						/home/lsa/Desktop/tone-generator-test/CFugue/test/../src 
						/home/lsa/Desktop/tone-generator-test/CFugue/test/../src/QtVuMeter 
						/home/lsa/Desktop/tone-generator-test/CFugue/build/test 
						/home/lsa/Desktop/tone-generator-test/CFugue/test/../src/3rdparty/tse3/src)

#target_link_libraries(alerts /usr/local/lib/libCFuguedu.a /usr/local/lib/libjdkmididu.a)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -rdynamic -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_BIND_TO_CURRENT_CRT_VERSION")
SET (CFUGUE_LINKFLAGS -lasound -lpthread) 

INCLUDE_DIRECTORIES(${CFUGUE_INCLUDE_DIR})
ADD_LIBRARY(alerts STATIC /usr/local/lib/libCFuguedu.a /usr/local/lib/libjdkmididu.a)
TARGET_LINK_LIBRARIES (alerts ${CFUGUE_LINKFLAGS} -lcurl) 
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR}) #source files list


PLAYER_ADD_PLUGIN_DRIVER (alerts SOURCES alerts.cc CFLAGS ${CFUGUE_CFLAGS}) #source files list
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libalerts.so DESTINATION /usr/local/lib64/) 


