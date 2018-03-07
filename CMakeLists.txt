cmake_minimum_required(VERSION 2.8)
project(Donnie)

#IF (POLICY CMP0048) # in CMake 3.0.0+
#  CMAKE_POLICY (SET CMP0048 OLD) # keep PROJECT() from clearing VERSION variables
#ENDIF (POLICY CMP0048)
#set(PROJECT_VERSION "0.1")

cmake_policy(SET CMP0017 OLD)

#####################################
# check DONNIE_PATH
if(DEFINED ENV{DONNIE_PATH})
   SET(DONNIE_PATH "$ENV{DONNIE_PATH}")
   message(STATUS "DONNIE_PATH = ${DONNIE_PATH}")
else()
   message(FATAL_ERROR "DONNIE_PATH is NOT defined")
endif()

#####################################
# check DONNIE_PATH
if(DEFINED ENV{DONNIE_SOURCE_PATH})
   SET(DONNIE_SOURCE_PATH "$ENV{DONNIE_SOURCE_PATH}")
else()
   SET(DONNIE_SOURCE_PATH "$ENV{PWD}")
endif()
message(STATUS "DONNIE_SOURCE_PATH = ${DONNIE_SOURCE_PATH}")

#####################################
# important varaiable used to find packages 
if(DEFINED ENV{PKG_CONFIG_PATH})
	#SET (PKG_CONFIG_PATH $ENV{PKG_CONFIG_PATH})
	# get the CMake module paths from the environmental variable CMAKE_MODULE_PATH 
	set(AUX_PKG_CONFIG_PATH "$ENV{PKG_CONFIG_PATH}")
	# PKG_CONFIG_PATH must be a list, with ; as delimitation character
	string(REPLACE ":" ";" PKG_CONFIG_PATH ${AUX_PKG_CONFIG_PATH})	
	MESSAGE(STATUS "PKG_CONFIG_PATH = ${PKG_CONFIG_PATH}")
else()
   message(FATAL_ERROR "PKG_CONFIG_PATH is NOT defined. Set where to find the pkg-config files.")
endif()

if(DEFINED ENV{CMAKE_MODULE_PATH})
	#SET (CMAKE_MODULE_PATH $ENV{CMAKE_MODULE_PATH})
	# get the CMake module paths from the environmental variable CMAKE_MODULE_PATH 
	set(AUX_CMAKE_MODULE_PATH "$ENV{CMAKE_MODULE_PATH}")
	# CMAKE_MODULE_PATH must be a list, with ; as delimitation character
	string(REPLACE ":" ";" CMAKE_MODULE_PATH ${AUX_CMAKE_MODULE_PATH})
	MESSAGE(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
else()
   message(FATAL_ERROR "CMAKE_MODULE_PATH is NOT defined. Set where to find the cmake files.")
endif()

#####################################
# cmake options
OPTION (BUILD_DOCS "Build Documents" OFF)
OPTION (BUILD_EXAMPLES "Build Examples" OFF)

#####################################
# Set the default build type
IF (NOT CMAKE_BUILD_TYPE)
  SET (CMAKE_BUILD_TYPE "Release" CACHE STRING 
    "Choose the type of build, options are: Release (default) Debug Profile" FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)
STRING(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
MESSAGE( STATUS "Build type ${CMAKE_BUILD_TYPE}" )

#####################################
# TODO: support multiple languages for for docs and GoDonnie Interpreter. It does not do anything yet
SET (DONNIE_LANGUAGE "en" CACHE STRING "Define the language for Donnie`s docs and commands. The options are: en (default) pt-br es" FORCE)
STRING(TOUPPER ${DONNIE_LANGUAGE} DONNIE_LANGUAGE)
MESSAGE( STATUS "Donnie`s docs and commands language: ${DONNIE_LANGUAGE}" )

#####################################
# some modules are required only for the robot computer, other only for the desktop.
# this variable is used to detect if it is the robot or desktop computer.
MESSAGE (STATUS)
MESSAGE (STATUS "----- Identifying OS and Processor -----")
MESSAGE( STATUS "System type: ${CMAKE_SYSTEM_NAME}" )
MESSAGE( STATUS "Processor type: ${CMAKE_SYSTEM_PROCESSOR}" )

if (${CMAKE_CROSSCOMPILING} OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    SET (ROBOT_COMPUTER TRUE)
else ()
    SET (ROBOT_COMPUTER FALSE)
endif ()

MESSAGE( STATUS "Robot Computer type: ${ROBOT_COMPUTER}" )
MESSAGE (STATUS "----- Identification Done -----")

#TODO : pesquisar https://cmake.org/cmake/help/v3.0/module/ProcessorCount.html
#TODO: support multiple operating systems, like windows
#IF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#    # Mac OS X specific code
#    SET(OperatingSystem "Mac OS X")
#
#    IF(${CMAKE_SYSTEM_PROCESSOR } MATCHES "Intel")
#        # Intel Mac OS X specific code
#        SET(Processor "Intel")
#    ENDIF(${CMAKE_SYSTEM_PROCESSOR } MATCHES "Intel")
#
#ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

######################################
# Externally Compile/Install Player
######################################
# http://mirkokiefer.com/blog/2013/03/cmake-by-example/
# TODO: exemplo p chamar outros makefiles ou cmakes externos
#ExternalProject_Add(project_docs
#  URL http://luajit.org/download/LuaJIT-2.0.1.tar.gz
#  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/luajit-2.0.1
#  CONFIGURE_COMMAND ""
#  BUILD_COMMAND make
#  INSTALL_COMMAND make install PREFIX=${CMAKE_CURRENT_BINARY_DIR}/luajit-2.0.1
#)
#
#ExternalProject_Get_Property(project_luajit install_dir)
#
#add_library(luajit STATIC IMPORTED)
#set_property(TARGET luajit PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libluajit-5.1.a)
#add_dependencies(luajit project_luajit)
#
#add_executable(myapp main.c)
#
#include_directories(${install_dir}/include/luajit-2.0)
#target_link_libraries(myapp luajit)

######################################
# Externally Compile/Install Stage
#IF(NOT ROBOT_COMPUTER)
#	# compile only for the desktop computer, not for RPi
#	add_subdirectory(stage)
#ENDIF()

######################################
# Externally Compile/Install Raspicam
#IF(ROBOT_COMPUTER)
#	# compile only for the RPi, not for desktop computer
#	add_subdirectory(raspicam)
#ENDIF()

######################################
# Compile/Install Donnie

# Set boost environment
FIND_PACKAGE (Boost REQUIRED COMPONENTS locale)
if (Boost_FOUND)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
endif ()

MESSAGE (STATUS)
MESSAGE (STATUS "----- Processing client makefiles -----")
add_subdirectory(client)
MESSAGE (STATUS "----- Client files Done -----")

MESSAGE (STATUS)
MESSAGE (STATUS "----- Processing driver makefiles -----")
add_subdirectory(driver)
MESSAGE (STATUS "----- Drivers Done -----")

# TODO compile Arduino firmware with CMake
# https://github.com/queezythegreat/arduino-cmake
# http://playground.arduino.cc/Code/CmakeBuild
#MESSAGE (STATUS)
#MESSAGE (STATUS "----- Processing Arduino firmware -----")
#add_subdirectory(firmware)
#MESSAGE (STATUS "----- Arduino Done -----")

MESSAGE (STATUS)
MESSAGE (STATUS "----- Processing doc makefiles -----")
add_subdirectory(docs)
MESSAGE (STATUS "----- Docs Done -----")

MESSAGE (STATUS)
MESSAGE (STATUS "----- Processing resources makefiles -----")
add_subdirectory(resources)
add_subdirectory(install)
add_subdirectory(simulation_models)
MESSAGE (STATUS "----- Resources Done -----")
