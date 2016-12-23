#!/bin/bash
#####################
# Author: Alexandre Amory
# Date: October/2016, Decemeber/2016
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
export DONNIE_PATH="@DONNIE_PATH@"
# donnie source dir
export DONNIE_SOURCE_PATH="@DONNIE_SOURCE_PATH@"
# dir used to run Donnie
export DONNIE_EXEC_PATH="${DONNIE_PATH}/test/stage"
#required to run donnie
export PATH=${PATH}:${DONNIE_PATH}/bin
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${DONNIE_PATH}/lib/player
export LD_LIBRARY_PATH=/usr/lib:${LD_LIBRARY_PATH}
# Opencv lib path
export LD_LIBRARY_PATH=/usr/lib/arm-linux-gnueabihf/:${LD_LIBRARY_PATH}
# Player lib path
export LD_LIBRARY_PATH=/usr/local/lib/:${LD_LIBRARY_PATH}

# required to compile donnie
# run 'sudo find / -name "*.pc" -type f' to find all the pc files for pkg-config
# run 'sudo find / -name "*.cmake" -type f' to find all the cmake files for cmake
export PKG_CONFIG_PATH=/usr/lib/pkgconfig:/usr/local/lib/pkgconfig:/usr/lib/arm-linux-gnueabihf/pkgconfig:/usr/share/pkgconfig
export CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}:/usr/share/OpenCV/:/usr/share/cmake-2.8/Modules/Compiler/:/usr/share/cmake-2.8/Modules/Platform/:/usr/share/cmake-2.8/Modules/:/usr/local/lib/cmake/:/usr/local/share/cmake/Modules/

alias ll='ls -lah'

function donnie_check_var {
	if [ -z "$DONNIE_PATH" ]; then
		echo -e "${RED}ERROR:${NC} Need to set DONNIE_PATH environment variable\n"
		echo -e "Example: export DONNIE_PATH=/opt/donnie\n"
		exit 1
	fi  
	if [ -z "$DONNIE_SOURCE_PATH" ]; then
		echo -e "${RED}ERROR:${NC} Need to set DONNIE_SOURCE_PATH environment variable\n"
		echo -e "Example: export DONNIE_SOURCE_PATH=~/donnie/donnie-assistive-robot-sw\n"
		exit 1
	fi  	
}

function donnie_update {
	donnie_check_var
	echo "Atualizando o Donnie no diretório ${DONNIE_SOURCE_PATH}"
    cd "${DONNIE_SOURCE_PATH}/build"
	git pull origin devel
	cmake ..
	make -j 8
	sudo make install
	cd -
	echo "Atualização concluída"
}

function donnie_player(){
  donnie_check_var
  echo "Executando Donnie software no diretório ${DONNIE_EXEC_PATH}"
  #cd "${DONNIE_EXEC_PATH}"
  xterm -e "player ${DONNIE_EXEC_PATH}/server_donnie.cfg" &
  sleep 5
  xterm -e "${DONNIE_PATH}/bin/alerts" &
  sleep 3
  echo "Donnie executando ..."
}

function donnie_kill(){
  killall player
  echo "Fechando Donnie ..."
}
