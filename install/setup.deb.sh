#!/bin/bash
#####################
# Author: Alexandre Amory
# Date: October/2016, May/2017
# Laboratorio de Sistemas Autonomos - FACIN - PUCRS University
# Description:
#   This is a setup script for debian package. 
#   Make sure this file is executed in your ~/.bashrc file
# How to Execute:

# Donnie user environment variables
# donnie instalation dir
export DONNIE_PATH="/opt/donnie"
# dir used to run Donnie
export DONNIE_EXEC_PATH="${DONNIE_PATH}/test/stage"
# donnie IP address. Use localhost for simulation mode. When the robot 
# is used, this variable should have the robot`s IP address
export DONNIE_IP=localhost
export DONNIE_PORT=6665

#required to run donnie
export PATH=${PATH}:${DONNIE_PATH}/bin
export PATH=/opt/player/bin:/opt/stage/bin:${PATH}
export LD_LIBRARY_PATH=/usr/lib:/usr/local/lib/:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=/opt/player/lib64:/opt/stage/lib64/:${LD_LIBRARY_PATH}
# Opencv lib path
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:${LD_LIBRARY_PATH}
# Player lib path
export LD_LIBRARY_PATH=${DONNIE_PATH}/lib/player:${LD_LIBRARY_PATH}


function donnie_check_var {
	if [ -z "$DONNIE_PATH" ]; then
		echo -e "${RED}ERROR:${NC} Need to set DONNIE_PATH environment variable\n"
		echo -e "Example: export DONNIE_PATH=/opt/donnie\n"
		exit 1
	fi  	
}

function donnie_player(){
  donnie_check_var
  echo "Executando Donnie software no diretório ${DONNIE_EXEC_PATH}"
  #nohup player ${DONNIE_EXEC_PATH}/server_donnie.cfg &> /dev/null &
  
  #Parse options
  if [ "$1" = "-nogui" ] 
  then
    xterm -e "player ${DONNIE_EXEC_PATH}/server_donnie_nogui.cfg &> /dev/null" &
	sleep 5
	xterm -e "${DONNIE_PATH}/bin/alerts  &> /dev/null" &
	sleep 3
	echo "Donnie executando ..."
  elif [ "$1" = "--help" ] || [ "$1" = "-h" ] 
  then
	echo "Donnie Player launcher"
	echo "USAGE:  donnie_player [options]"
	echo -e "\nWhere [options] can be:"
	echo "  -h, --help         : print this message."
	echo "  --nogui            : launch player interface without graphic interface."
	#echo "  -c                : acess launch config."
  else  #default
	xterm -e "player ${DONNIE_EXEC_PATH}/server_donnie.cfg &> /dev/null" &
	sleep 5
	xterm -e "${DONNIE_PATH}/bin/alerts  &> /dev/null" &
	sleep 3
	echo "Donnie executando ..."
  fi
}

function donnie_kill(){
  killall player
  echo "Fechando Donnie ..."
}
