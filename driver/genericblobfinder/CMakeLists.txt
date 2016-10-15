CMAKE_MINIMUM_REQUIRED (VERSION 2.8 FATAL_ERROR)
SET (DRIVERNAME "GenericBlobfinder")
PROJECT (${DRIVERNAME})

#set(PROJECT_VERSION "0.1.2")

message(STATUS "Configuring '${DRIVERNAME}'")

#########
# check DONNIE_PATH
#########
if(DEFINED ENV{DONNIE_PATH})
   SET(DONNIE_PATH "$ENV{DONNIE_PATH}")
   #message(STATUS "DONNIE_PATH = ${DONNIE_PATH}")
else()
   message(FATAL_ERROR "DONNIE_PATH is NOT defined")
endif()

# add the cmake path for raspicam and player
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/local/lib/cmake/ /usr/local/share/cmake/Modules)
#MESSAGE(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

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

message(STATUS "OpenCV Library -- ${OpenCV_LIBS}")
message(STATUS "OpenCV Include -- ${OpenCV_INCLUDE_DIRS}")

#########
# set Player cmake modules
#########
INCLUDE (UsePlayerPlugin)

PLAYER_ADD_PLUGIN_DRIVER (${DRIVERNAME} SOURCES ${DRIVERNAME}.cc) 
TARGET_LINK_LIBRARIES (${DRIVERNAME} ${OpenCV_LIBS}) 
# ${OpenCV_INCLUDE_DIRS} was already inserted. no need to do it again
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})

# Install library and example files
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${DRIVERNAME}.so DESTINATION ${DONNIE_PATH}/lib/player/) 
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/server_${DRIVERNAME}.cfg DESTINATION ${DONNIE_PATH}/test/${DRIVERNAME})

# compile the driver example
ADD_SUBDIRECTORY(client)
