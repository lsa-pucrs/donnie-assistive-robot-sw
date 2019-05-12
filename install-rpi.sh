#!/bin/bash
#####################
# Author: Alexandre Amory
# Date: September/2016, December/2016
# Laboratorio de Sistemas Autonomos - FACIN - PUCRS University
# Description:
#   This script installs all the required depedencies (packages) in the RPi.
#   No compilation is performed on RPi. 
#   Run this before cross compilation (install-cross-pi.sh) !
# How to Execute:
#
# Note that this script is meant for Donnie`s developers, not for users, 
# since one will execute this script only if the software running on RPi is changed.
# Normal users will burn a pre-build RPi image for their Donnie robot.
#
# copy this script to the RPi
#   $ scp install-rpi.sh pi@<rpi-ip>:~
# log on RPi and run the script
#   $ ssh pi@<rpi-ip>
#   $ sudo ./install-rpi.sh

# TODO: tenho duvidas se preciso instalar *-dev ou se seria necessario somente as libs sem headers.
# se *-dev for necessario para cross compiling, entao eu acho q eles nao seriam necessarios na 
# distribuicao final. depois de cross compilado, seria possivel remover *-dev, e somente instalar 
# as libs

# Any subsequent(*) commands which fail will cause the shell script to exit immediately
set -e

#defensive script
#http://www.davidpashley.com/articles/writing-robust-shell-scripts/#id2382181

RED='\033[0;31m'
GREEN='\033[0;32m'
ORANGE='\033[0;33m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${RED}Running Donnie Compilation Script for Raspberry Pi ... ${NC}\n"

if [ -z "$DONNIE_PATH" ]; then
    echo -e "${RED}ERROR:${NC} Need to set DONNIE_PATH environment variable\n"
    echo -e "Example: export DONNIE_PATH=/opt/donnie\n"
    exit 1
fi  
echo -e "DONNIE_PATH=${DONNIE_PATH}\n"
export DONNIE_SOURCE_PATH=${PWD}/donnie-assistive-robot-sw
echo -e "DONNIE_SOURCE_PATH=${DONNIE_SOURCE_PATH}\n"

sudo apt-get update
sudo apt-get install -y lsb-release

##################################################
# check the supported OS version and distribution
##################################################
OS=$(lsb_release -si)
VER=$(lsb_release -sr)
OSNAME=$(lsb_release -sc)

case ${OS} in 
	Raspbian)
		echo -e "${ORANGE}WARNING:${NC} Raspbian is recommended only for Raspberry Pi\n"
		case ${VER} in 
			8.0)
				echo -e "${GREEN}NOTE:${NC} ${OS} - ${VER} (${OSNAME}) is the recommended OS version.\n"
				;;
			*)
				# Handle other OS versions here
				echo -e "${ORANGE}WARNING:${NC} ${OS} - ${VER} (${OSNAME}) is not a recommended OS version. You might get errors and some programming experience is required to compile Donnie. \n"
				echo -e "${GREEN}NOTE:${NC} Raspbian Version 8 (Jessie) is the recommended version for ${OS}\n"
				exit 1;
			 ;;
		esac
		;;	
	*)
     # Handle other distributions here
		echo -e "${RED}ERROR:${NC} ${OS} is not a supported OS\n"
		echo -e "${GREEN}NOTE:${NC} Raspbian Version 8 (Jessie) is recommended for Donnie's computer (Raspberry Pi)\n"
		exit 1;
     ;;
esac

NUM_CORES=`cat /proc/cpuinfo | grep processor | wc -l`
echo -e "${GREEN}NOTE:${NC} This computer has ${NUM_CORES} cores ...\n"

##################################################
# Setting up Raspberry Pi
##################################################
echo -e "${GREEN}Setting up Raspberry Pi ... ${NC}\n"
#Desativa o login serial da Raspberry (Para evitar o conflito da serial do Arduino)
#Source: http://spellfoundry.com/sleepypi/settingarduinoideraspbian/
sudo systemctl mask serialgetty@ttyAMA0.service

#Configurando o desligamento da raspberry (Shutdown) por interrupção do pino GPIO4
#Referencia(No link tem outros uso para a GPIO Zero também): http://bennuttall.com/gpio-zero-developing-a-new-friendly-python-api-for-physical-computing/ 
sudo apt-get install python3-gpiozero python-gpiozero

# '$' means the last line, 'i' means insert before the current line, so '$i' means insert before the last line.
sudo sed -i -e '$i \sleep 10\n' /etc/rc.local ## this sleep is required to play festival. sometimes rc.locals plays too early
sudo sed -i -e '$i \nohup sudo python '"${DONNIE_PATH}"'/scripts/softshutdown.py &\n' /etc/rc.local

# script used to Donnie tell its IP address at startup 
sudo apt-get install -y festival
sudo sed -i -e '$i \nohup sudo python '"${DONNIE_PATH}"'/scripts/speechIP.py &\n' /etc/rc.local

echo -e "${GREEN}Raspberry Pi Set Up Completed !!!!${NC}\n"

##################################################
# install commom packages
##################################################
sudo apt-get install -y build-essential

# nice to have, not mandatory
sudo apt-get install -y geany

#compilation utils
echo -e "${GREEN}Installing Compilation Utils ... ${NC}\n"
sudo apt-get install -y autoconf
sudo apt-get install -y gdb
sudo apt-get install -y cmake
sudo apt-get install -y cmake-curses-gui
sudo apt-get install -y git
sudo apt-get install -y pkg-config

# Jessie installs cmake 3.0 by the default, but lubuntu 14.04 uses cmake 2.8
# this gives some weird warnings when running cmake 3.0 on rpi.
# so we downgraded the cmake to 2.8, used in wheezy
# https://www.raspbian.org/RaspbianRepository
# <=========================
#wget https://archive.raspbian.org/raspbian.public.key -O - | sudo apt-key add -
#echo "deb http://archive.raspbian.org/raspbian wheezy main contrib non-free" |  sudo tee --append /etc/apt/sources.list > /dev/null
#echo "deb-src http://archive.raspbian.org/raspbian wheezy main contrib non-free"  |  sudo tee --append /etc/apt/sources.list > /dev/null
#sudo apt-get update

#apt-cache madison cmake

#sudo apt-get install -y cmake-data=2.8.9-1
#sudo apt-get install -y cmake=2.8.9-1
#sudo apt-get install -y cmake-curses-gui=2.8.9-1


##################################################
# install Donnie depedencies
##################################################
# Donnie's depedencies
echo -e "${GREEN}Installing Donnie Dependencies on RPi ... ${NC}\n"
#to compile soxplayer driver
#sudo apt-get install -y sox
sudo apt-get install -y libsox-dev 
#to compile gtts driver
#sudo apt-get install -y curl TODO acho q essa lib nao eh mais necessaria. Igor, testar !!!
sudo apt-get install -y libcurl4-openssl-dev
#Instalar o TIMIDITY para poder usar os canais virtuais de MIDI (Virtual MIDI Port) e usar notas musicais no autofalante 
sudo apt-get install -y timidity
# std terminal used in several linux distributions
sudo apt-get install xterm

#Testando saida do auto falante
#$speaker-test -t sine -f 1000 -c 2

#Selecionar a saida para o jack p2
#sudo amixer cset numid=3 1 # headphones
#sudo amixer cset numid=3 0 # Auto
#sudo amixer cset numid=3 2 # HDMI

#Controle do volume
#sudo alsamixer

#Tocando um mp3
#omxplayer mp3name.mp3


##################################################
# Donwloading source code 
##################################################
# if [ ! -d "./Player" ]; then
# 	echo -e "${GREEN}Downloading Player source code from GitHub... ${NC}\n"
# 	git clone https://github.com/lsa-pucrs/Player.git
# fi

# if [ ! -d "./raspicam" ]; then
# 	echo -e "${GREEN}Downloading Raspicam source code from GitHub... ${NC}\n"
# 	git clone https://github.com/lsa-pucrs/raspicam.git
# fi

# if [ ! -d "./donnie-assistive-robot-sw" ]; then
# 	echo -e "${GREEN}Downloading Donnie source code from GitHub... ${NC}\n"
# 	git clone -b devel https://github.com/lsa-pucrs/donnie-assistive-robot-sw.git
# fi

##################################################
# set environment variables
##################################################
#required to run donnie
export PATH=${PATH}:${DONNIE_PATH}/bin
export LD_LIBRARY_PATH=/usr/lib:/usr/local/lib/:${LD_LIBRARY_PATH}
# Opencv lib path
export LD_LIBRARY_PATH=/usr/lib/arm-linux-gnueabihf/:${LD_LIBRARY_PATH}
# Player lib path
export LD_LIBRARY_PATH=/usr/local/lib/:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${DONNIE_PATH}/lib/player:${LD_LIBRARY_PATH}

# required to compile donnie
# run 'sudo find / -name "*.pc" -type f' to find all the pc files for pkg-config
# run 'sudo find / -name "*.cmake" -type f' to find all the cmake files for cmake
case "${VER}" in 
	8.0)
		export 	CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}:/usr/share/cmake-2.8/Modules/:/usr/share/cmake-2.8/Modules/Platform/:/usr/share/cmake-2.8/Modules/Compiler/:/usr/local/share/cmake/Modules:/usr/local/lib/cmake/:/usr/lib/fltk/:/usr/local/share/OpenCV/:/usr/share/OpenCV/
		;;

	*)
		export CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}:/usr/share/cmake-3.5/Modules/:/usr/share/cmake-3.5/Modules/Platform/:/usr/share/cmake-3.5/Modules/Compiler/:/usr/local/share/cmake/Modules:/usr/local/lib/cmake/Stage/:/usr/lib/fltk/
		;;
esac
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/:/usr/lib/pkgconfig:/usr/lib/arm-linux-gnueabihf/pkgconfig/:/usr/share/pkgconfig/:${PKG_CONFIG_PATH}

##################################################
# Compile and install Player
##################################################
source ./3rd-party/player.sh

# return to the base dir
cd $PWD

##################################################
# install openCV 3.2
##################################################
source ./3rd-party/opencv.sh
# TODO Igor aparentemente esse link mostra como instalar uma versao pre-compilada do opencv 3
# https://www.pyimagesearch.com/2018/09/19/pip-install-opencv/
# tem esse outro
# https://github.com/jabelone/OpenCV-for-Pi

# return to the base dir
cd $PWD

##################################################
# Compiling raspicam driver
##################################################
source ./3rd-party/raspicam.sh

# return to the base dir
cd $PWD

##################################################
# Compiling and installing Donnie
##################################################
cd "${DONNIE_SOURCE_PATH}"
mkdir -p build
cd build
echo -e "${GREEN}Configuring Donnie ... ${NC}\n"
# CMAKE_SYSTEM_PROCESSOR=arm is required to avoid compiling things not used by the robot computer
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_PROCESSOR=arm ..
echo -e "${GREEN}Compiling Donnie ... ${NC}\n"
make -j ${NUM_CORES} 
sudo make install
echo -e "${GREEN}Donnie installed !!!! ${NC}\n"

##################################################
# set environment variables for terminals
##################################################
echo "source $DONNIE_PATH/setup.sh" >> ~/.bashrc

##################################################
# uninstall all dev packages to save space
##################################################
echo -e "${GREEN}Cleaning the cache ... ${NC}\n"
sudo apt-get -y autoclean
sudo apt-get -y autoremove

echo -e "${GREEN}End of installation !!!! ${NC}\n"
