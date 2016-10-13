#!/bin/bash
#####################
# Author: Alexandre Amory
# Date: September/2016
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

RED='\033[0;31m'
GREEN='\033[0;32m'
ORANGE='\033[0;33m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${RED}Running Donnie Compilation Script for Raspberry Pi ... ${NC}\n"

if [ -z "$DONNIE_PATH" ]; then
    echo -e "${RED}ERROR:{NC} Need to set DONNIE_PATH environment variable\n"
    echo -e "Example: export DONNIE_PATH=/opt/donnie\n"
    exit 1
fi  
echo -e "DONNIE_PATH=${DONNIE_PATH}\n"

##################################################
# check the supported OS version and distribution
##################################################
OS=$(lsb_release -si)
VER=$(lsb_release -sr)
OSNAME=$(lsb_release -sc)

case ${OS} in 
	Rasbian )
		echo -e "${ORANGE}WARNING:{NC} Raspbian is recommended only for Raspberry Pi\n"
		case ${VER} in 
			8.0 )
				echo -e "${GREEN}NOTE:{NC} ${OS} - ${VER} (${OSNAME}) is the recommended OS version.\n"
				;;
			* )
				# Handle other OS versions here
				echo -e "${ORANGE}WARNING:{NC} ${OS} - ${VER} (${OSNAME}) is not a recommended OS version. You might get errors and some programming experience is required to compile Donnie. \n"
				echo -e "${GREEN}NOTE:{NC} Raspbian Version 8 (Jessie) is the recommended version for ${OS}\n"
				exit 1;
			 ;;
		esac		
	* )
     # Handle other distributions here
		echo -e "${RED}ERROR:{NC} ${OS} is not a supported OS\n"
		echo -e "${GREEN}NOTE:{NC} Raspbian Version 8 (Jessie) is recommended for Donnie's computer (Raspberry Pi)\n"
		exit 1;
     ;;
esac


##################################################
# install commom packages
##################################################
apt-get update
#apt-get install -y build-essential

# nice to have, not mandatory
apt-get install -y geany

#compilation utils
echo -e "${GREEN}Installing Compilation Utils ... ${NC}\n"
#apt-get install -y autoconf
#apt-get install -y cmake
#apt-get install -y cmake-curses-gui
#apt-get install -y git
sudo apt-get install -y pkg-config

##################################################
# set environment variables
##################################################
# TODO correct the path for RPI
mkdir ${DONNIE_PATH}
echo '# Donnie definitions' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DONNIE_PATH/lib/player' >> ~/.bashrc
echo 'export LC_ALL=C' >> ~/.bashrc
export LD_LIBRARY_PATH=/usr/lib:/usr/local/lib/:$LD_LIBRARY_PATH
# Opencv lib path
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:$LD_LIBRARY_PATH
# Player lib path
export LD_LIBRARY_PATH=/usr/local/lib64/:$LD_LIBRARY_PATH
# run 'sudo find / -name "*.pc" -type f' to find all the pc files for pkg-config
echo 'export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib64/pkgconfig/:/usr/lib/pkgconfig/:/usr/lib/x86_64-linux-gnu/pkgconfig/:/usr/share/pkgconfig/' >> ~/.bashrc
# run 'sudo find / -name "*.cmake" -type f' to find all the pc files for pkg-config
echo 'export CMAKE_MODULE_PATH=$CMAKE_MODULE_PATH:/usr/local/lib64/cmake/Stage/:/usr/local/share/cmake/Modules/:/usr/share/cmake-2.8/Modules/:/usr/share/OpenCV/' >> ~/.bashrc

##################################################
# install Player depedencies
##################################################
# TODO remove the Stage related packages
echo -e "${GREEN}Installing Player/Stage Dependencies ... ${NC}\n"
#sudo apt-get install -y libfltk1.1-dev 
#sudo apt-get install -y freeglut3-dev  # <= talvez nao precise no player se desabilitar pmap
sudo apt-get install -y libpng12-dev 
#sudo apt-get install -y libltdl-dev 
#sudo apt-get install -y libdb5.1-stl
sudo apt-get install -y libgnomecanvasmm-2.6-dev
#sudo apt-get install -y python-gnome2
sudo apt-get install -y libboost-all-dev  # overkill, the actually required libraries are boostthread, boostsignal, boostsystem
#sudo apt-get install -y libboost-dev
# old OpenCV for older Player drivers
# sudo apt-get install -y libopencv-dev # <= install all packages ... not required
# sudo apt-get install -y libopencv-dev libopencv-core-dev libcv-dev libcvaux-dev libhighgui-dev
sudo apt-get install -y libopencv-core-dev libhighgui-dev libcv-dev libopencv-dev 
# alsa - sound player
# http://player-stage-gazebo.10965.n7.nabble.com/CCmake-cannot-find-the-existing-asoundlib-h-for-ALSA-driver-td11198.html
sudo apt-get install -y libasound2-dev
# alsa alsa-tools  alsa-utils
# for pmap
#sudo apt-get install -y libgsl0-dev libxmu-dev
# for python bindings for Player clients - 
# It is not recommended to use python due to limitations in the bindings. 
# Things that work on a C/C++ client might not work on a Python client.
#sudo apt-get install -y python-dev swig
# PostGIS for a Player driver
sudo apt-get install -y libpq-dev libpqxx-dev


##################################################
# install Donnie depedencies
##################################################

# Donnie's depedencies
echo -e "${GREEN}Installing Donnie Dependencies on RPi ... ${NC}\n"
#to compile soxplayer driver
#sudo apt-get install -y sox
sudo apt-get install -y libsox-dev 
#to compile gtts driver
#sudo apt-get install -y curl
sudo apt-get install -y libcurl4-openssl-dev


##################################################
# uninstall all dev packages to save space
##################################################
echo -e "${GREEN}Removing compilation tools and cleaning the cache ... ${NC}\n"
apt-get purge -y build-essential
apt-get purge -y autoconf
apt-get purge -y cmake
apt-get purge -y cmake-curses-gui
apt-get purge -y git
apt-get purge -y pkg-config
apt-get autoclean
apt-get autoremove

echo -e "${GREEN}End of installation !!!! ${NC}\n"


