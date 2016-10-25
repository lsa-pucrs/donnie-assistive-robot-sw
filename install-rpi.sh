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
    echo -e "${RED}ERROR:${NC} Need to set DONNIE_PATH environment variable\n"
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

##################################################
# install commom packages
##################################################
sudo apt-get update
sudo apt-get install -y build-essential

# nice to have, not mandatory
sudo apt-get install -y geany

#compilation utils
echo -e "${GREEN}Installing Compilation Utils ... ${NC}\n"
sudo apt-get install -y autoconf
#sudo apt-get install -y cmake
#sudo apt-get install -y cmake-curses-gui
sudo apt-get install -y git
sudo apt-get install -y pkg-config

# Jessie installs cmake 3.0 by the default, but lubuntu 14.04 uses cmake 2.8
# this gives some weird warnings when running cmake 3.0 on rpi.
# so we downgraded the cmake to 2.8, used in wheezy
# https://www.raspbian.org/RaspbianRepository
wget https://archive.raspbian.org/raspbian.public.key -O - | sudo apt-key add -
echo "deb http://archive.raspbian.org/raspbian wheezy main contrib non-free" |  sudo tee --append /etc/apt/sources.list > /dev/null
echo "deb-src http://archive.raspbian.org/raspbian wheezy main contrib non-free"  |  sudo tee --append /etc/apt/sources.list > /dev/null
sudo apt-get update
#apt-cache madison cmake
sudo apt-get install -y cmake-data=2.8.9-1
sudo apt-get install -y cmake=2.8.9-1
sudo apt-get install -y cmake-curses-gui=2.8.9-1

##################################################
# set environment variables
##################################################
source ./install/setup-rpi.sh
echo "source $DONNIE_PATH/setup-rpi.sh" >> ~/.bashrc

##################################################
# install Player depedencies
##################################################
# TODO remove the Stage related packages
echo -e "${GREEN}Installing Player Dependencies ... ${NC}\n"
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
# Donwloading source code 
##################################################
echo -e "${GREEN}Downloading Player source code from GitHub... ${NC}\n"
cd ~/Downloads
git clone https://github.com/lsa-pucrs/Player.git

echo -e "${GREEN}Downloading Raspicam source code from GitHub... ${NC}\n"
git clone https://github.com/lsa-pucrs/raspicam.git

echo -e "${GREEN}Downloading Donnie source code from GitHub... ${NC}\n"
git clone -b devel https://github.com/lsa-pucrs/donnie-assistive-robot-sw.git

##################################################
# Compile and install Player
##################################################
cd Player
echo -e "${GREEN}Patching Player for Donnie ... ${NC}\n"
patch -p1 < patch/festival/festival.patch
patch -p1 < patch/install/player_3.0.2_14.04.patch
echo -e "${GREEN}Patching Player for Donnie ... ${NC}\n"
patch -p1 < ../donnie-assistive-robot-sw/proxies/instalationSoundProxy.patch
mkdir build
cd build
echo -e "${GREEN}Configuring Player ... ${NC}\n"
#disablingCMAKE_INSTALL_PREFIX all the unused drivers and utils...
cmake -DCMAKE_INSTALL_PREFIX=/usr/local \
	-DCMAKE_BUILD_TYPE=Release \
	-DDEBUG_LEVEL=NONE \
	-DBUILD_PYTHONC_BINDINGS:BOOL=OFF \
	-DBUILD_EXAMPLES:BOOL=OFF \
	-DBUILD_UTILS_XMMS:BOOL=OFF \
	-DBUILD_UTILS_PMAP:BOOL=OFF \
	-DBUILD_UTILS_PLAYERNAV:BOOL=OFF \
	-DBUILD_UTILS_PLAYERPRINT:BOOL=OFF \
	-DBUILD_UTILS_PLAYERPROP:BOOL=OFF \
	-DBUILD_UTILS_PLAYERV:BOOL=OFF \
	-DBUILD_UTILS_PLAYERVCR:BOOL=OFF \
	-DBUILD_UTILS_PLAYERWRITEMAP:BOOL=OFF \
	-DBUILD_UTILS_LOGSPLITTER:BOOL=OFF \
	-DENABLE_DRIVER_ACCEL_CALIB:BOOL=OFF \
	-DENABLE_DRIVER_ACTS:BOOL=OFF \
	-DENABLE_DRIVER_AIOTOSONAR:BOOL=OFF \
	-DENABLE_DRIVER_AMTECPOWERCUBE:BOOL=OFF \
	-DENABLE_DRIVER_AODV:BOOL=OFF \
	-DENABLE_DRIVER_BITLOGIC:BOOL=OFF \
	-DENABLE_DRIVER_CAMERA1394:BOOL=OFF \
	-DENABLE_DRIVER_CANONVCC4:BOOL=OFF \
	-DENABLE_DRIVER_CLODBUSTER:BOOL=OFF \
	-DENABLE_DRIVER_CMDSPLITTER:BOOL=OFF \
	-DENABLE_DRIVER_CMUCAM2:BOOL=OFF \
	-DENABLE_DRIVER_CMVISION:BOOL=OFF \
	-DENABLE_DRIVER_CREATE:BOOL=OFF \
	-DENABLE_DRIVER_DEADSTOP:BOOL=OFF \
	-DENABLE_DRIVER_EPUCK:BOOL=OFF \
	-DENABLE_DRIVER_ER1:BOOL=OFF \
	-DENABLE_DRIVER_ERRATIC:BOOL=OFF \
	-DENABLE_DRIVER_FLOCKOFBIRDS:BOOL=OFF \
	-DENABLE_DRIVER_GARMINNMEA:BOOL=OFF \
	-DENABLE_DRIVER_IMAGESEQ:BOOL=OFF \
	-DENABLE_DRIVER_IMGCMP:BOOL=OFF \
	-DENABLE_DRIVER_INHIBITOR:BOOL=OFF \
	-DENABLE_DRIVER_INSIDEM300:BOOL=OFF \
	-DENABLE_DRIVER_IWSPY:BOOL=OFF \
	-DENABLE_DRIVER_KARTOWRITER:BOOL=OFF \
	-DENABLE_DRIVER_KHEPERA:BOOL=OFF \
	-DENABLE_DRIVER_LASERBAR:BOOL=OFF \
	-DENABLE_DRIVER_LASERBARCODE:BOOL=OFF \
	-DENABLE_DRIVER_LASERCSPACE:BOOL=OFF \
	-DENABLE_DRIVER_LASERCUTTER:BOOL=OFF \
	-DENABLE_DRIVER_LASERPOSEINTERPO:BOOL=OFF \
	-DENABLE_DRIVER_LASERPTZCLOUD:BOOL=OFF \
	-DENABLE_DRIVER_LASERRESCAN:BOOL=OFF \
	-DENABLE_DRIVER_LASERSAFE:BOOL=OFF \
	-DENABLE_DRIVER_LOCALBB:BOOL=OFF \
	-DENABLE_DRIVER_MBICP:BOOL=OFF \
	-DENABLE_DRIVER_MICA2:BOOL=OFF \
	-DENABLE_DRIVER_MICROSTRAIN:BOOL=OFF \
	-DENABLE_DRIVER_MOTIONMIND:BOOL=OFF \
	-DENABLE_DRIVER_MRICP:BOOL=OFF \
	-DENABLE_DRIVER_NOMAD:BOOL=OFF \
	-DENABLE_DRIVER_OBOT:BOOL=OFF \
	-DENABLE_DRIVER_OCEANSERVER:BOOL=OFF \
	-DENABLE_DRIVER_P2OS:BOOL=OFF \
	-DENABLE_DRIVER_PASSTHROUGH:BOOL=OFF \
	-DENABLE_DRIVER_PBSLASER:BOOL=OFF \
	-DENABLE_DRIVER_POSTGIS:BOOL=OFF \
	-DENABLE_DRIVER_POSTLOG:BOOL=OFF \
	-DENABLE_DRIVER_PTU46:BOOL=OFF \
	-DENABLE_DRIVER_RELAY:BOOL=OFF \
	-DENABLE_DRIVER_RFLEX:BOOL=OFF \
	-DENABLE_DRIVER_ROBOTEQ:BOOL=OFF \
	-DENABLE_DRIVER_ROBOTRACKER:BOOL=OFF \
	-DENABLE_DRIVER_ROOMBA:BOOL=OFF \
	-DENABLE_DRIVER_RS4LEUZE:BOOL=OFF \
	-DENABLE_DRIVER_RT3XXX:BOOL=OFF \
	-DENABLE_DRIVER_SEARCHPATTERN:BOOL=OFF \
	-DENABLE_DRIVER_SEGWAYRMP400:BOOL=OFF \
	-DENABLE_DRIVER_SERIALSTREAM:BOOL=OFF \
	-DENABLE_DRIVER_SERIO:BOOL=OFF \
	-DENABLE_DRIVER_SICKLDMRS:BOOL=OFF \
	-DENABLE_DRIVER_SICKLMS200:BOOL=OFF \
	-DENABLE_DRIVER_SICKLMS400:BOOL=OFF \
	-DENABLE_DRIVER_SICKNAV200:BOOL=OFF \
	-DENABLE_DRIVER_SICKRFI341:BOOL=OFF \
	-DENABLE_DRIVER_SICKS3000:BOOL=OFF \
	-DENABLE_DRIVER_SKYETEKM1:BOOL=OFF \
	-DENABLE_DRIVER_SONYEVID30:BOOL=OFF \
	-DENABLE_DRIVER_SPHEREPTZ:BOOL=OFF \
	-DENABLE_DRIVER_SUPPRESSOR:BOOL=OFF \
	-DENABLE_DRIVER_UPCBARCODE:BOOL=OFF \
	-DENABLE_DRIVER_WBR914:BOOL=OFF \
	-DPLAYER_BUILD_TESTS:BOOL=OFF \
	-DBUILD_CC_TESTS:BOOL=OFF \
	.. 
echo -e "${GREEN}Compiling Player ... ${NC}\n"
make
sudo make install
echo -e "${GREEN}Player installed !!!! ${NC}\n"

##################################################
# Compiling raspicam driver
##################################################
#https://solderspot.wordpress.com/2016/02/04/cross-compiling-for-raspberry-pi-part-ii/
cd ../../raspicam
mkdir build
cd build
echo -e "${GREEN}Configuring raspicam ... ${NC}\n"
cmake -DCMAKE_BUILD_TYPE=Release \
	..
echo -e "${GREEN}Compiling raspicam ... ${NC}\n"
make
sudo make install
echo -e "${GREEN}Raspicam installed !!!! ${NC}\n"

##################################################
# Compiling and installing Donnie
##################################################
cd ../../donnie-assistive-robot-sw
mkdir build
cd build
echo -e "${GREEN}Configuring Donnie ... ${NC}\n"
# CMAKE_SYSTEM_PROCESSOR=arm is required to avoid compiling things not used by the robot computer
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_PROCESSOR=arm ..
echo -e "${GREEN}Compiling Donnie ... ${NC}\n"
make
sudo make install
echo -e "${GREEN}Donnie installed !!!! ${NC}\n"

##################################################
# uninstall all dev packages to save space
##################################################
echo -e "${GREEN}Cleaning the cache ... ${NC}\n"
sudo apt-get -y autoclean
sudo apt-get -y autoremove

echo -e "${GREEN}End of installation !!!! ${NC}\n"
