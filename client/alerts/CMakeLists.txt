CMAKE_MINIMUM_REQUIRED (VERSION 2.8 FATAL_ERROR)
SET (CLIENTNAME "alerts")
PROJECT (${CLIENTNAME})

message(STATUS "Configuring '${CLIENTNAME}'")

#########
# check DONNIE_PATH
#########
if(DEFINED ENV{DONNIE_PATH})
   SET(DONNIE_PATH "$ENV{DONNIE_PATH}")
   #message(STATUS "DONNIE_PATH = ${DONNIE_PATH}")
else()
   message(FATAL_ERROR "DONNIE_PATH is NOT defined")
endif()

#########
# check depedencies - boost locale
#########
FIND_PACKAGE (Boost REQUIRED COMPONENTS locale)
IF( NOT Boost_FOUND )
  MESSAGE( FATAL_ERROR "Libboost-locale not found. Please install 'sudo apt-get install libboost-locale-dev'. aborting" )
ENDIF()

#########
# set Player cmake modules
#########
# Include this CMake module to get most of the settings needed to build
#SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")

INCLUDE (UsePlayerC++)

PLAYER_ADD_PLAYERCPP_CLIENT (${CLIENTNAME} 
	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/alerts.cc ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/utils.cpp
	INCLUDEDIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/)

TARGET_LINK_LIBRARIES (${CLIENTNAME} ${Boost_LIBRARIES}) 
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIR})

# Install executables with -x permission 
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${CLIENTNAME} DESTINATION ${DONNIE_PATH}/bin) 
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/* DESTINATION ${DONNIE_PATH}/test/${CLIENTNAME})
