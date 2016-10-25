#!/bin/bash
#####################
# Author: Alexandre Amory
# Date: October/2016
# Laboratorio de Sistemas Autonomos - FACIN - PUCRS University
# Description:
#   This is a setup script for Donnie`s embedded computer (RPi). 
#   Make sure this file is executed in your ~/.bashrc file
# How to Execute:

#general definitions
alias ll='ls -lah'
export LC_ALL=C

# Donnie user environment variables
# donnie instalation dir
export DONNIE_PATH=/opt/donnie
# donnie source dir
export DONNIE_SOURCE_PATH=/home/lsa/Downloads/donnie-assistive-robot-sw

#required to run donnie
export PATH=$PATH:$DONNIE_PATH/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DONNIE_PATH/lib/player
export LD_LIBRARY_PATH=/usr/lib:$LD_LIBRARY_PATH
# Opencv lib path
export LD_LIBRARY_PATH=/usr/lib/arm-linux-gnueabihf/:$LD_LIBRARY_PATH
# Player lib path
export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH

# required to compile donnie
# run 'sudo find / -name "*.pc" -type f' to find all the pc files for pkg-config
# run 'sudo find / -name "*.cmake" -type f' to find all the cmake files for cmake
export PKG_CONFIG_PATH=/usr/lib/pkgconfig:/usr/local/lib/pkgconfig:/usr/lib/arm-linux-gnueabihf/pkgconfig:/usr/share/pkgconfig
export CMAKE_MODULE_PATH=$CMAKE_MODULE_PATH:/usr/share/OpenCV/:/usr/share/cmake-2.8/Modules/Compiler/:/usr/share/cmake-2.8/Modules/Platform/:/usr/share/cmake-2.8/Modules/:/usr/local/lib/cmake/:/usr/local/share/cmake/Modules/

alias ll='ls -lah'

function donnie_robot_update {
	echo "TO BE DONE !!!"
}

function donnie_robot_compile {
	echo "TO BE DONE !!!"
}

function donnie_robot_install {
	echo "TO BE DONE !!!"
}
