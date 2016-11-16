#!/bin/bash
#####################
# Author: Alexandre Amory
# Date: September/2016
# Laboratorio de Sistemas Autonomos - FACIN - PUCRS University
# Description:
#   This script installs Donnie software in the host computer (Player, Stage, Donnie drivers).
# How to Execute (host computer):
# go to Donnie source code dir
#   $ cd <donnie source>
# run this script
#   $ sudo ./install.sh <parameters>
# parameters:
#   SIMULATION_ONLY= 1 / 0 # set to 1 if you dont have Donnie robot

RED='\033[0;31m'
GREEN='\033[0;32m'
ORANGE='\033[0;33m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${RED}Running Donnie Compilation Script for Lubuntu 14.04 ... ${NC}\n"

if [ -z "$DONNIE_PATH" ]; then
    echo -e "${RED}ERROR:${NC} Need to set DONNIE_PATH environment variable\n"
    echo -e "Example: export DONNIE_PATH=/opt/donnie\n"
    exit 1
fi  
echo -e "DONNIE_PATH=${DONNIE_PATH}\n"

##################################################
# check the supported OS version and distribution
##################################################
#TODO compile Player for windows
# https://models.slf.ch/p/meteoio/page/Compiling-MeteoIO/
OS=$(lsb_release -si)
VER=$(lsb_release -sr)
OSNAME=$(lsb_release -sc)

case ${OS} in 
	Ubuntu)
		echo -e "${GREEN}NOTE:${NC} Ubuntu is only recommended for Destop computer, not VMs, and not Raspberry Pi\n"
		echo -e "${GREEN}NOTE:${NC} Ubuntu 14.04 (trusty) is the recommended version for ${OS}\n"
		case ${VER} in 
			14.04)
				echo -e "${GREEN}NOTE:${NC} ${OS} - ${VER} (${OSNAME} is the recommended OS version.\n"
				;;
			*)
				# Handle other OS versions here
				echo -e "${ORANGE}WARNING:${NC} ${OS} - ${VER} (${OSNAME} is not a recommended OS version. You might get errors and some programming experience is required to compile Donnie. \n"
				echo -e "Press CTRL+C to exit or Enter to continue the installation:"
				read
			    ;;
		esac		
		;;
	Lubuntu)
		echo -e "${GREEN}NOTE:${NC} Lubuntu is recommended for both Destop computers and VMs, but not for Raspberry Pi\n"
		echo -e "${GREEN}NOTE:${NC} Lubuntu 14.04 (trusty) is the recommended version for ${OS}\n"
		case ${VER} in 
			14.04)
				echo -e "${GREEN}NOTE:${NC} ${OS} - ${VER} (${OSNAME} is the recommended OS version.\n"
				;;
			*)
				# Handle other OS versions here
				echo -e "${ORANGE}WARNING:${NC} ${OS} - ${VER} (${OSNAME} is not a recommended OS version. You might get errors and some programming experience is required to compile Donnie. \n"
				echo -e "Press CTRL+C to exit or Enter to continue the installation:"
				read
			    ;;
		esac
		;;			
	*)
     # Handle other distributions here
		echo -e "${RED}ERROR:${NC} ${OS} - ${VER} (${OSNAME} is not a supported OS\n"
		echo -e "${GREEN}NOTE:${NC}Ubuntu/Lubuntu 14.04 (trusty) are the recommended OS for Donnie's host computer\n"
		echo -e "Press CTRL+C to exit or Enter to continue the installation:"
		read
        ;;
esac

NUM_CORES=`cat /proc/cpuinfo | grep processor | wc -l`
echo -e "${GREEN}NOTE:${NC} This computer has ${NUM_CORES} cores ...\n"

##################################################
# install commom packages
##################################################
# required to install Oracle's java
sudo apt-add-repository ppa:webupd8team/java
sudo apt-get update
sudo apt-get install -y build-essential

# update VBoX Additions
sudo apt-get install -y module-assistant
sudo apt-get install -y virtualbox-guest-dkms
sudo m-a prepare

# nice to have, not mandatory
sudo apt-get install -y geany
sudo apt-get install -y google-chrome-stable

#compilation utils
echo -e "${GREEN}Installing Compilation Utils ... ${NC}\n"
sudo apt-get install -y autoconf
sudo apt-get install -y cmake
sudo apt-get install -y cmake-curses-gui
sudo apt-get install -y git
sudo apt-get install -y pkg-config

##################################################
# set environment variables
##################################################
source ./install/setup.sh
echo "source $DONNIE_PATH/setup.sh" >> ~/.bashrc

##################################################
# install Player/Stage depedencies
##################################################
echo -e "${GREEN}Installing Player/Stage Dependencies ... ${NC}\n"
sudo apt-get install -y libfltk1.1-dev 
sudo apt-get install -y freeglut3-dev 
sudo apt-get install -y libpng12-dev 
sudo apt-get install -y libltdl-dev 
#libltdl7 
sudo apt-get install -y libdb5.1-stl
sudo apt-get install -y libgnomecanvasmm-2.6-dev
sudo apt-get install -y python-gnome2
sudo apt-get install -y libboost-all-dev  # overkill, the actually required libraries are boostthread, boostsignal, boostsystem
# old OpenCV for older Player drivers
sudo apt-get install -y libopencv-dev libopencv-core-dev libcv-dev libcvaux-dev libhighgui-dev
# alsa - sound player
# http://player-stage-gazebo.10965.n7.nabble.com/CCmake-cannot-find-the-existing-asoundlib-h-for-ALSA-driver-td11198.html
sudo apt-get install -y libasound2-dev
# alsa alsa-tools  alsa-utils
# for pmap
sudo apt-get install -y libgsl0-dev libxmu-dev
# for python bindings for Player clients - 
# It is not recommended to use python due to limitations in the bindings. 
# Things that work on a C/C++ client might not work on a Python client.
sudo apt-get install -y python-dev swig
# PostGIS for a Player driver
sudo apt-get install -y libpq-dev libpqxx-dev

##################################################
# Donwloading source code 
##################################################
echo -e "${GREEN}Downloading Player source code from GitHub... ${NC}\n"
cd ~/Downloads
git clone https://github.com/lsa-pucrs/Player.git

echo -e "${GREEN}Downloading Stage source code from GitHub... ${NC}\n"
git clone https://github.com/lsa-pucrs/Stage.git

echo -e "${GREEN}Downloading Raspicam source code from GitHub... ${NC}\n"
git clone https://github.com/lsa-pucrs/raspicam.git

echo -e "${GREEN}Downloading Donnie source code from GitHub... ${NC}\n"
git clone -b devel https://github.com/lsa-pucrs/donnie-assistive-robot-sw.git

##################################################
# Compile and install Player/Stage 
##################################################
cd Player
echo -e "${GREEN}Patching Player for Lubuntu 14.04 ... ${NC}\n"
patch -p1 < patch/festival/festival.patch
patch -p1 < patch/install/player_3.0.2_14.04.patch
patch -p1 < ../donnie-assistive-robot-sw/proxies/instalationSoundProxy.patch
echo -e "${GREEN}Patching Player for Donnie ... ${NC}\n"
mkdir build
cd build
# Mandatory
# DEBUG_LEVEL=NONE <==== important !!!
# Recommended: Build the Python bindings for the C client library
# BUILD_PYTHONCPP_BINDINGS:BOOL=ON
# BUILD_PYTHONC_BINDINGS:BOOL=ON
echo -e "${GREEN}Configuring Player for Lubuntu 14.04 ... ${NC}\n"
cmake -DCMAKE_BUILD_TYPE=Release -DDEBUG_LEVEL=NONE -BUILD_PYTHONC_BINDINGS:BOOL=ON ..
echo -e "${GREEN}Compiling Player for Lubuntu 14.04 ... ${NC}\n"
make -j ${NUM_CORES} 
sudo make install
echo -e "${GREEN}Player installed !!!! ${NC}\n"

cd ../../Stage
mkdir build
cd build
# Mandatory
# CMAKE_BUILD_TYPE=release <==== important !!!
echo -e "${GREEN}Configuring Stage for Lubuntu 14.04 ... ${NC}\n"
cmake -DCMAKE_BUILD_TYPE=Release ..
echo -e "${GREEN}Compiling Stage for Lubuntu 14.04 ... ${NC}\n"
make -j ${NUM_CORES} 
sudo make install
echo -e "${GREEN}Stage installed !!!! ${NC}\n"

##################################################
# install Donnie depedencies
# compile and install Donnie
##################################################
# Donnie's depedencies
echo -e "${GREEN}Installing Donnie Dependencies ... ${NC}\n"
#to compile soxplayer driver
#sudo apt-get install -y sox
# TODO: check if any of these packages are actually required for donnie
#  188  sudo apt-get install libsoxr0
#  190  sudo apt-get install sox
#  222  sudo apt-get install xfce4-mixer gstreamer0.10-alsa
#  225  sudo apt-get install pulseaudio pavucontrol
sudo apt-get install -y libsox-dev 
#to compile gtts driver
#sudo apt-get install -y curl
sudo apt-get install -y libcurl4-openssl-dev
#to compile GoDonnie grammart interpreter
sudo apt-get install -y libreadline-dev
sudo apt-get install -y oracle-java8-installer
sudo apt-get install -y libantlr3c-dev
#to compile docs
sudo apt-get install -y doxygen
# uses 3.058 MB in disk - not recommended for VMs
#sudo apt-get install -y texlive-full
# uses 494MB in disk
sudo apt-get install -y texlive texlive-lang-english texlive-lang-portuguese
# uses 558MB in disk
sudo apt-get install -y texlive-latex-extra
# saves some 700MB in disk by removing docs
sudo apt-get --purge remove -y tex.\*-doc$


cd ../../donnie-assistive-robot-sw
mkdir build
cd build
echo -e "${GREEN}Configuring Donnie for Lubuntu 14.04 ... ${NC}\n"
cmake -DCMAKE_BUILD_TYPE=Release \
	-DBUILD_EXAMPLES=ON \
	-DBUILD_DOCS=ON  \
	-DBUILD_DOXYGEN=ON \
	-DBUILD_DOXYGEN_PDF=ON \
	-DBUILD_MANUAL=ON \
    ..
echo -e "${GREEN}Compiling Donnie for Lubuntu 14.04 ... ${NC}\n"
make -j ${NUM_CORES} 
sudo make install
echo -e "${GREEN}Donnie installed !!!! ${NC}\n"

echo -e "${GREEN}End of installation !!!! ${NC}\n"

##################################################
# uninstall all dev packages to save space
##################################################
echo -e "${GREEN}Cleaning the cache ... ${NC}\n"
sudo apt-get -y autoclean
sudo apt-get -y autoremove

echo -e "${GREEN}End of installation !!!! ${NC}\n"
