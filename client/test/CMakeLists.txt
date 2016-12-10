CMAKE_MINIMUM_REQUIRED (VERSION 2.8 FATAL_ERROR)

message(STATUS "Configuring 'client tests'")

#########
# check DONNIE_PATH
#########
if(DEFINED ENV{DONNIE_PATH})
   SET(DONNIE_PATH "$ENV{DONNIE_PATH}")
   #message(STATUS "DONNIE_PATH = ${DONNIE_PATH}")
else()
   message(FATAL_ERROR "DONNIE_PATH is NOT defined")
endif()

# add here any additional test source files
FILE( GLOB clientTestSrcs "*.cc" )
#SET (clientTestSrcs  test_rangers.cc
#    )

# install all source files
INSTALL (FILES ${clientTestSrcs} DESTINATION ${DONNIE_PATH}/test/client)
# install additional files
INSTALL(FILES donnie.cfg donnie_sim.cfg Makefile DESTINATION ${DONNIE_PATH}/test/client)

# compile the client examples
IF ( BUILD_EXAMPLES )

	# Include this CMake module to get most of the settings needed to build
	SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")
	INCLUDE (UsePlayerC++)

	# add EACH executable and sources here
	PLAYER_ADD_PLAYERCPP_CLIENT (test_rangers 
		SOURCES test_rangers.cc ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/utils.cpp
		INCLUDEDIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/
		)
	INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/test_rangers DESTINATION ${DONNIE_PATH}/test/client )

ENDIF ()
