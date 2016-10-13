CMAKE_MINIMUM_REQUIRED (VERSION 2.4 FATAL_ERROR)
SET (CLIENTNAME "GoDonnie")
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

# check the compiler
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(FATAL "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
# required to support 'acentos' in Portuguese
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=iso-8859-1")


# Include this CMake module to get most of the settings needed to build
SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")

#########
# check depedencies - java -lreadline -ltermcap  -lantlr3c -pthread
#########

# required to run antlr
find_package(Java REQUIRED)
#include(UseJava)

# https://raw.githubusercontent.com/opencv/opencv/2.4/cmake/OpenCVConfig.cmake
#FIND_PACKAGE(OpenCV 2 REQUIRED)
#MESSAGE(STATUS "OpenCV_DIR = ${OpenCV_DIR}")
#IF(OpenCV_FOUND)
#	MESSAGE(STATUS "OPENCV Found !")
#ELSE()
#	MESSAGE(FATAL_ERROR "OPENCV NOT FOUND IN YOUR SYSTEM")
#ENDIF()
#message(STATUS "OpenCV Library -- ${OpenCV_LIBS}")
#message(STATUS "OpenCV Include -- ${OpenCV_INCLUDE_DIRS}")

INCLUDE (UsePlayerC++)

# run antlr
add_custom_target(GoDonnieParser 
					# Java creates the files on wrong dir. So it is required to copy this file 
					# otherwise the paser source codes will be created in the CMAKE_CURRENT_SOURCE_DIR
                  COMMAND rsync --progress -r -u  ${CMAKE_CURRENT_SOURCE_DIR}/GoDonnie.g ${CMAKE_CURRENT_BINARY_DIR}
                  COMMAND java -jar ${CMAKE_CURRENT_SOURCE_DIR}/Antlr/antlr-3.2.jar ${CMAKE_CURRENT_BINARY_DIR}/GoDonnie.g
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  SOURCES ${CMAKE_CURRENT_BINARY_DIR}/GoDonnie.g)

# hack to break the depedency
# https://samthursfield.wordpress.com/2015/11/21/cmake-dependencies-between-targets-and-files-and-custom-commands/
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/GoDonnieParser.c
    PROPERTIES GENERATED TRUE
    )
    
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/GoDonnieLexer.c
    PROPERTIES GENERATED TRUE
    )
    
PLAYER_ADD_PLAYERCPP_CLIENT (${CLIENTNAME} 
	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Compiler.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/DonnieClient.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/GoDonnie.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp ${CMAKE_CURRENT_BINARY_DIR}/GoDonnieLexer.c  ${CMAKE_CURRENT_BINARY_DIR}/GoDonnieParser.c
	INCLUDEDIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

TARGET_LINK_LIBRARIES (${CLIENTNAME} readline termcap  antlr3c pthread) 
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})

# godonnie depends on GoDonnieParser
add_dependencies(GoDonnie GoDonnieParser)

# Install library and example files
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CLIENTNAME} DESTINATION ${DONNIE_PATH}/bin) 
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/square.gd DESTINATION ${DONNIE_PATH}/test/${CLIENTNAME})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/test.gd DESTINATION ${DONNIE_PATH}/test/${CLIENTNAME})
