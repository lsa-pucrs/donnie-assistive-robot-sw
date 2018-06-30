SET(THE_TARGET "visualodometry")
SET(LIBDEST "/opt/player-stage/src/player-3.0.2/build/examples/plugins/")

# TO DO: this cmake seems incomple. there must be a depedency for raspicam driver. check the  cmakelists of the other drivers to keep the standard
CMAKE_MINIMUM_REQUIRED (VERSION 2.4 FATAL_ERROR)
PROJECT (${THE_TARGET})

FIND_PACKAGE(OpenCV REQUIRED)
IF(OpenCV_FOUND)
	MESSAGE(STATUS "COMPILING OPENCV AND RASPICAM")
ELSE()
	MESSAGE(FATAL_ERROR "OPENCV OR RASPICAM NOT FOUND IN YOUR SYSTEM")
ENDIF()


# Include this CMake module to get most of the settings needed to build
SET (CMAKE_MODULE_PATH "@CMAKE_INSTALL_PREFIX@/share/cmake/Modules")
INCLUDE (UsePlayerPlugin)

PLAYER_ADD_PLUGIN_DRIVER (${THE_TARGET} SOURCES ${THE_TARGET}.cc)

INCLUDE_DIRECTORIES (${PLAYERCORE_EXTRA_INCLUDE_DIRS})
LINK_DIRECTORIES (${PLAYERCORE_EXTRA_LINK_DIRS})
ADD_LIBRARY (${THE_TARGET} SHARED ${THE_TARGET}.cc)
TARGET_LINK_LIBRARIES (${THE_TARGET} playercore ${OpenCV_LIBS})

SET (exampleDestDir share/${PROJECT_NAME_LOWER}/examples/plugins/${THE_TARGET})
INSTALL (FILES ${THE_TARGET}.cc DESTINATION ${exampleDestDir} COMPONENT examples)
#INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${THE_TARGET}.so DESTINATION ${PLUGIN_PLAYERC_LIBRARY_DIRS})  #instala na pasta /usr/local/lib64
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${THE_TARGET}.so DESTINATION ${LIBDEST}${THE_TARGET}) 
