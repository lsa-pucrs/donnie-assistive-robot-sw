CMAKE_MINIMUM_REQUIRED (VERSION 2.8 FATAL_ERROR)
SET (DRIVERNAME "Raspicam_driver")
PROJECT (${DRIVERNAME})

message(STATUS "Configuring '${DRIVERNAME}'")

#IF (POLICY CMP0048) # in CMake 3.0.0+
#  CMAKE_POLICY (SET CMP0048 OLD) # keep PROJECT() from clearing VERSION variables
#ENDIF (POLICY CMP0048)

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


FIND_PACKAGE(raspicam REQUIRED
	PATHS /usr/local/lib/cmake/ )
IF(raspicam_CV_FOUND)
	MESSAGE(STATUS "RASPICAM Found !")
ELSE()
	MESSAGE(FATAL_ERROR "RASPICAM NOT FOUND IN YOUR SYSTEM")
ENDIF()
message(STATUS "RASPICAM Library -- ${raspicam_CV_LIBS}")
message(STATUS "RASPICAM Include -- /usr/local/include")


#########
# set Player cmake modules
#########
INCLUDE (UsePlayerPlugin)

PLAYER_ADD_PLUGIN_DRIVER (${DRIVERNAME} SOURCES ${DRIVERNAME}.cc) 
TARGET_LINK_LIBRARIES (${DRIVERNAME} ${OpenCV_LIBS} ${raspicam_CV_LIBS}) 
# ${OpenCV_INCLUDE_DIRS} was already inserted. no need to do it again
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})

# Install library and example cfg file
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${DRIVERNAME}.so DESTINATION ${DONNIE_PATH}/lib/player/) 
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${DRIVERNAME}.cfg DESTINATION ${DONNIE_PATH}/test/${DRIVERNAME})

# compile the driver example
ADD_SUBDIRECTORY(client)
