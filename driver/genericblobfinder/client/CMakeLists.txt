CMAKE_MINIMUM_REQUIRED (VERSION 2.8 FATAL_ERROR)
PROJECT (GenericBlobfinder-test)

#########
# check DONNIE_PATH
#########
if(DEFINED ENV{DONNIE_PATH})
   SET(DONNIE_PATH "$ENV{DONNIE_PATH}")
   #message(STATUS "DONNIE_PATH = ${DONNIE_PATH}")
else()
   message(FATAL_ERROR "DONNIE_PATH is NOT defined")
endif()

# add the cmake path for player
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/local/share/cmake/Modules)

#########
# check depedencies - opencv 2
#########
# https://raw.githubusercontent.com/opencv/opencv/2.4/cmake/OpenCVConfig.cmake
FIND_PACKAGE(OpenCV 2 REQUIRED)
MESSAGE(STATUS "OpenCV_DIR = ${OpenCV_DIR}")

IF(OpenCV_FOUND)
	MESSAGE(STATUS "OPENCV Found !")
ELSE()
	
	MESSAGE(FATAL_ERROR "OPENCV NOT FOUND IN YOUR SYSTEM")
ENDIF()

#message(STATUS "OpenCV Library -- ${OpenCV_LIBS}")
#message(STATUS "OpenCV Include -- ${OpenCV_INCLUDE_DIRS}")

#########
# set Player cmake modules
#########
INCLUDE (UsePlayerC++)

PLAYER_ADD_PLAYERCPP_CLIENT (GenericBlobfinder-test SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Blobfindertest.cpp)
TARGET_LINK_LIBRARIES (GenericBlobfinder-test ${OpenCV_LIBS}) 
# ${OpenCV_INCLUDE_DIRS} was already inserted. no need to do it again
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/GenericBlobfinder-test
		DESTINATION ${DONNIE_PATH}/test/GenericBlobfinder
)

INSTALL(FILES Makefile Blobfindertest.cpp
        DESTINATION ${DONNIE_PATH}/test/GenericBlobfinder
)
