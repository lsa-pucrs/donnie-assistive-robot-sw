CMAKE_MINIMUM_REQUIRED (VERSION 2.8 FATAL_ERROR)
PROJECT (soxplayer-test)

#########
# check DONNIE_PATH
#########
if(DEFINED ENV{DONNIE_PATH})
   SET(DONNIE_PATH "$ENV{DONNIE_PATH}")
   #message(STATUS "DONNIE_PATH = ${DONNIE_PATH}")
else()
   message(FATAL_ERROR "DONNIE_PATH is NOT defined")
endif()

# Include this CMake module to get most of the settings needed to build
SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")

INCLUDE (UsePlayerC++)

PLAYER_ADD_PLAYERCPP_CLIENT (soxplayer-test SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/client-test.cc)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/soxplayer-test
		DESTINATION ${DONNIE_PATH}/test/soxplayer
)
