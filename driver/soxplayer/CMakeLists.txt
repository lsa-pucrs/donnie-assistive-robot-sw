CMAKE_MINIMUM_REQUIRED (VERSION 2.4 FATAL_ERROR)
SET (DRIVERNAME "soxplayer")
PROJECT (${DRIVERNAME})

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

#########
# check DONNIE_PATH
#########
if(DEFINED ENV{DONNIE_PATH})
   SET(DONNIE_PATH "$ENV{DONNIE_PATH}")
   message(STATUS "DONNIE_PATH = ${DONNIE_PATH}")
else()
   message(FATAL_ERROR "DONNIE_PATH is NOT defined")
endif()

#########
# check depedencies - libsox
#########
find_path( LSOX_INCLUDE_DIR sox.h DOC "Libsox include dir" )
find_library( LSOX_LIB 
	NAMES sox 
	HINTS /usr/lib/x86_64-linux-gnu/
	DOC "Libsox lib" REQUIRED)

IF( NOT LSOX_LIB )
  MESSAGE( FATAL_ERROR "libSoX not found. Please install 'sudo apt-get install libsox-dev'. aborting" )
ENDIF()
message(STATUS "SoX Library -- ${LSOX_LIB}")
message(STATUS "SoX Include -- ${LSOX_INCLUDE_DIR}")

#########
# set Player cmake modules
#########
# normal cmake dir is /usr/share/cmake-2.8/Modules/
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/local/share/cmake/Modules")
INCLUDE (UsePlayerPlugin)

PLAYER_ADD_PLUGIN_DRIVER (${DRIVERNAME} SOURCES ${DRIVERNAME}.cc) 
TARGET_LINK_LIBRARIES (${DRIVERNAME} -lsox) 
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR} ${LSOX_INCLUDE_DIR})

# Install library and example files
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${DRIVERNAME}.so DESTINATION ${DONNIE_PATH}/lib/player/) 
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/client-test/client-test.cc DESTINATION ${DONNIE_PATH}/test/${DRIVERNAME})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/client-test/CMakeLists.txt DESTINATION ${DONNIE_PATH}/test/${DRIVERNAME})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/server_sound.cfg DESTINATION ${DONNIE_PATH}/test/${DRIVERNAME})

# compile the driver example
IF ( BUILD_EXAMPLES )
  ADD_SUBDIRECTORY(client-test)
ENDIF ()
