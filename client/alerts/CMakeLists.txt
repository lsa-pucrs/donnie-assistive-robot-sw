CMAKE_MINIMUM_REQUIRED (VERSION 2.8 FATAL_ERROR)
SET (CLIENTNAME "alerts")
PROJECT (${CLIENTNAME})

message(STATUS "Configuring '${CLIENTNAME}'")

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

# Include this CMake module to get most of the settings needed to build
SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")

INCLUDE (UsePlayerC++)

PLAYER_ADD_PLAYERCPP_CLIENT (${CLIENTNAME} 
	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/alerts.cc ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/utils.cpp
	INCLUDEDIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/)

TARGET_LINK_LIBRARIES (${CLIENTNAME} sox) 
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR} ${LSOX_INCLUDE_DIR})

# Install executables with -x permission 
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${CLIENTNAME} DESTINATION ${DONNIE_PATH}/bin) 
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/* DESTINATION ${DONNIE_PATH}/test/${CLIENTNAME})
