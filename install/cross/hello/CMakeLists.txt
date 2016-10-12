#####################
# Author: Alexandre Amory
# Date: September/2016
# Laboratorio de Sistemas Autonomos - FACIN - PUCRS University
# Description:
#   Example CMake file to perform cross compilation for Donnie.
# How to Execute (host computer):
#   $ mkdir build
#   $ cd build
#   $ cmake -DCMAKE_TOOLCHAIN_FILE=$DONNIE_PATH/rpi.cmake ..
#   $ make
#   $ make install
# If the RPI_ROOT_PATH is mounted, then it also sends the exec to the RPi`s /home/pi

cmake_minimum_required(VERSION 2.8)
project(HelloWorld)
add_executable(HelloWorld HelloWorld.cpp )
target_link_libraries(HelloWorld)

string(ASCII 27 Esc)
set(ColourReset "${Esc}[m")
set(Green       "${Esc}[32m")

# if the RPi root dir is mounted, then send the exec to the RPi`s /home/pi
# not required since i set -DCMAKE_INSTALL_PREFIX=$RPI_ROOT_PATH/home/pi
#if(EXISTS "$ENV{RPI_ROOT_PATH}/home/pi")
#	install(TARGETS HelloWorld DESTINATION $ENV{RPI_ROOT_PATH}/home/pi)
#	message(STATUS "${Green}Run make install to copy the executable to $ENV{RPI_ROOT_PATH}/home/pi ${ColourReset}")
#else()
#	install(TARGETS HelloWorld DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#	message(STATUS "${Green}Run make install to copy the executable to ${CMAKE_CURRENT_BINARY_DIR} ${ColourReset}")
#endif()
