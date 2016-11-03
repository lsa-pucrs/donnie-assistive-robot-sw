CMAKE_MINIMUM_REQUIRED (VERSION 2.8 FATAL_ERROR)
SET (DRIVERNAME "donnie")
PROJECT (${DRIVERNAME})

message(STATUS "Configuring '${DRIVERNAME}'")

#########
# check DONNIE_PATH
#########w
if(DEFINED ENV{DONNIE_PATH})
   SET(DONNIE_PATH "$ENV{DONNIE_PATH}")
   message(STATUS "DONNIE_PATH = ${DONNIE_PATH}")
else()
   message(FATAL_ERROR "DONNIE_PATH is NOT defined")
endif()


#########
# check depedencies - no depedencies
#########
#find_path( LCURL_INCLUDE_DIR curl.h 
#	PATH_SUFFIXES curl
#	DOC "Libcurl include dir" )
#find_library( LCURL_LIB 
#	NAMES curl 
#	
#	HINTS /usr/lib/x86_64-linux-gnu/
#	DOC "Libcurl lib" REQUIRED)
#
#IF( NOT LCURL_LIB )
#  MESSAGE( FATAL_ERROR "Libcurl not found. Please install 'sudo apt-get install libcurl4-openssl-dev'. aborting" )
#ENDIF()
#message(STATUS "Curl Library -- ${LCURL_LIB}")
#message(STATUS "Curl Include -- ${LCURL_INCLUDE_DIR}")

#########
# set Player cmake modules
#########
# normal cmake dir is /usr/share/cmake-2.8/Modules/
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/local/share/cmake/Modules")
INCLUDE (UsePlayerPlugin)

PLAYER_ADD_PLUGIN_DRIVER (${DRIVERNAME} SOURCES ${DRIVERNAME}.cc)   #INCLUDEDIRS ${CMAKE_CURRENT_SOURCE_DIR}
# add the includes and libs to build it
#TARGET_LINK_LIBRARIES (${DRIVERNAME} ${LCURL_LIB}) 
#INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR} ${LCURL_INCLUDE_DIR} )
TARGET_LINK_LIBRARIES (${DRIVERNAME}) 
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})


# Install library and example files
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${DRIVERNAME}.so DESTINATION ${DONNIE_PATH}/lib/player/) 
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/client-manual_control/client-donnie.cc DESTINATION ${DONNIE_PATH}/test/${DRIVERNAME})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/client-manual_control/CMakeLists.txt DESTINATION ${DONNIE_PATH}/test/${DRIVERNAME})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/server_donnie.cfg DESTINATION ${DONNIE_PATH}/test/${DRIVERNAME})

# compile the driver example
IF ( BUILD_EXAMPLES )
  ADD_SUBDIRECTORY(client-manual_control)
ENDIF ()
