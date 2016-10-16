#!/bin/bash
#####################
# Author: Alexandre Amory
# Date: October/2016
# Laboratorio de Sistemas Autonomos - FACIN - PUCRS University
# Description:
#   This is a setup script for Donnie`s desktop computer. 
#   Make sure this file is executed in your ~/.bashrc file
# How to Execute:

# Donnie user environment variables
# donnie instalation dir
export DONNIE_PATH=/opt/donnie
# donnie source dir
export DONNIE_SOURCE_PATH=/home/lsa/Downloads/donnie-assistive-robot-sw
# donnie IP address
export DONNIE_IP=192.168.0.6

#required to run donnie
export PATH=$PATH:$DONNIE_PATH/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DONNIE_PATH/lib/player
export LD_LIBRARY_PATH=/usr/lib:/usr/local/lib/:$LD_LIBRARY_PATH
# Opencv lib path
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:$LD_LIBRARY_PATH
# Player lib path
export LD_LIBRARY_PATH=/usr/local/lib64/:$LD_LIBRARY_PATH

# required to compile donnie
# run 'sudo find / -name "*.pc" -type f' to find all the pc files for pkg-config
# run 'sudo find / -name "*.cmake" -type f' to find all the cmake files for cmake
export CMAKE_MODULE_PATH=$CMAKE_MODULE_PATH:/usr/share/cmake-2.8/Modules/:/usr/share/cmake-2.8/Modules/Platform/:/usr/share/cmake-2.8/Modules/Compiler/:/usr/local/share/cmake/Modules:/usr/local/lib64/cmake/Stage/:/usr/lib/fltk/
export PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig/:/usr/lib/pkgconfig:/usr/lib/x86_64-linux-gnu/pkgconfig/:/usr/share/pkgconfig/:$PKG_CONFIG_PATH

# Donnie developer environment variables - just only is you are using cross compiling
export RPI_ROOT_PATH=/opt/raspberrypi/root
export RPI_TOOLS_PATH=/opt/raspberrypi/tools

alias ssh_donnie='ssh -X pi@$DONNIE_IP'

function donnie_login {
	ssh_donnie
}

function donnie_mount {
	sudo sshfs pi@$DONNIE_IP:/ $RPI_ROOT_PATH -o transform_symlinks -o allow_other
}

function donnie_umount {
	sudo umount $RPI_ROOT_PATH
}

function donnie_desktop_update {
	echo "TO BE DONE !!!"
}

function donnie_desktop_compile {
	echo "TO BE DONE !!!"
}

function donnie_desktop_install {
	echo "TO BE DONE !!!"
}

function donnie_robot_update {
	echo "TO BE DONE !!!"
}

function donnie_robot_compile {
	echo "TO BE DONE !!!"
}

function donnie_robot_install {
	echo "TO BE DONE !!!"
}