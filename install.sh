#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
ORANGE='\033[0;33m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${RED}Running Donnie Compilation Script for Lubuntu 14.04 ... ${NC}\n"

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

case ${OS} in 
	Ubuntu )
		echo -e "${ORANGE}WARNING:{NC} Ubuntu is only recommended for Destop computer, not VMs, and not Raspberry Pi\n"
		case ${VER} in 
			14.04 )
				echo -e "${GREEN}NOTE:{NC} ${OS} - ${VER} is the recommended OS version.\n"
				;;
			* )
				# Handle other OS versions here
				echo -e "${ORANGE}WARNING:{NC} ${OS} - ${VER} is not a recommended OS version. You might get errors and some programming experience is required to compile Donnie. \n"
				echo -e "${GREEN}NOTE:{NC} Version 14.04 is the recommended version for ${OS}\n"
				exit 1;
			 ;;
		esac		
		;;
	Lubuntu )
		echo -e "${ORANGE}WARNING:{NC} Lubuntu is recommended for both Destop computers and VMs, but not for Raspberry Pi\n"
		case ${VER} in 
			14.04 )
				echo -e "${GREEN}NOTE:{NC} ${OS} - ${VER} is the recommended OS version.\n"
				;;
			* )
				# Handle other OS versions here
				echo -e "${ORANGE}WARNING:{NC} ${OS} - ${VER} is not a recommended OS version. You might get errors and some programming experience is required to compile Donnie. \n"
				echo -e "${GREEN}NOTE:{NC} Version 14.04 is the recommended version for ${OS}\n"
				exit 1;
			 ;;
		esac		
	Rasbian )
		echo -e "${ORANGE}WARNING:{NC} Rasbian is recommended only for Raspberry Pi\n"
		case ${VER} in 
			14.04 )
				echo -e "${GREEN}NOTE:{NC} ${OS} - ${VER} is the recommended OS version.\n"
				;;
			* )
				# Handle other OS versions here
				echo -e "${ORANGE}WARNING:{NC} ${OS} - ${VER} is not a recommended OS version. You might get errors and some programming experience is required to compile Donnie. \n"
				echo -e "${GREEN}NOTE:{NC} Version 14.04 is the recommended version for ${OS}\n"
				exit 1;
			 ;;
		esac		
	* )
     # Handle other distributions here
		echo -e "${RED}ERROR:{NC} ${OS} is not a supported OS\n"
		echo -e "${GREEN}NOTE:{NC} Lubuntu is recommended for VMs or Desktops. Raspbian is recommended for Donnie's computer (Raspberry Pi)\n"
		exit 1;
     ;;
esac


##################################################
# install commom packages
##################################################

# required to install Oracle's java
sudo apt-add-repository ppa:webupd8team/java
apt-get update
apt-get install -y build-essential

# update VBoX Additions
apt-get install -y module-assistant
sudo m-a prepare

# nice to have, not mandatory
apt-get install -y geany
apt-get install -y google-chrome-stable


#compilation utils
echo -e "${GREEN}Installing Compilation Utils ... ${NC}\n"
apt-get install -y autoconf
apt-get install -y cmake
apt-get install -y cmake-curses-gui
apt-get install -y git
sudo apt-get install -y pkg-config

##################################################
# set environment variables
##################################################

# run 'sudo find / -name "*.pc" -type f' to find all the pc files for pkg-config
echo 'export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib64/pkgconfig/:/usr/lib/pkgconfig/:/usr/lib/x86_64-linux-gnu/pkgconfig/:/usr/share/pkgconfig/' >> ~/.bashrc
# run 'sudo find / -name "*.cmake" -type f' to find all the pc files for pkg-config
echo 'export CMAKE_MODULE_PATH=$CMAKE_MODULE_PATH:/usr/local/lib64/cmake/Stage/:/usr/local/share/cmake/Modules/:/usr/share/cmake-2.8/Modules/:/usr/share/OpenCV/' >> ~/.bashrc

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

export LD_LIBRARY_PATH=/usr/lib:/usr/local/lib/:$LD_LIBRARY_PATH
# Opencv lib path
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:$LD_LIBRARY_PATH
# Player lib path
export LD_LIBRARY_PATH=/usr/local/lib64/:$LD_LIBRARY_PATH
# Player and Stage pkg-config
export PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig/:$PKG_CONFIG_PATH


echo -e "${GREEN}Downloading Player sou
rce code from GitHub... ${NC}\n"
cd Downloads
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
echo -e "${GREEN}Patching Player for Donnie ... ${NC}\n"
patch -p1 < ../donnie-assistive-robot-sw/proxies/instalationSoundProxy.patch
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
make
make install
echo -e "${GREEN}Player installed !!!! ${NC}\n"

cd ../../Stage
mkdir build
cd build
# Mandatory
# CMAKE_BUILD_TYPE=release <==== important !!!
echo -e "${GREEN}Configuring Stage for Lubuntu 14.04 ... ${NC}\n"
cmake -DCMAKE_BUILD_TYPE=Release ..
echo -e "${GREEN}Compiling Stage for Lubuntu 14.04 ... ${NC}\n"
make
make install
echo -e "${GREEN}Stage installed !!!! ${NC}\n"


##################################################
# install Donnie depedencies
# compile and install Donnie
##################################################

# Donnie's depedencies
echo -e "${GREEN}Installing Donnie Dependencies ... ${NC}\n"
#to compile soxplayer driver
#sudo apt-get install -y sox
sudo apt-get install -y libsox-dev 
#to compile gtts driver
#sudo apt-get install -y curl
sudo apt-get install -y libcurl4-openssl-dev
#to compile GoDonnie grammart interpreter
sudo apt-get install -y libreadline-dev
sudo apt-get install -y oracle-java8-installer
sudo apt-get install -y libantlr3c-dev
#to compile docs
# uses 3.058 MB in disk - not recommended for VMs
#sudo apt-get install -y texlive-full
# uses 494MB in disk
sudo apt-get install -y texlive texlive-lang-english texlive-lang-portuguese
sudo apt-get install -y doxygen

cd ../../donnie-assistive-robot-sw
mkdir build
cd build
echo -e "${GREEN}Configuring Donnie for Lubuntu 14.04 ... ${NC}\n"
cmake -DCMAKE_BUILD_TYPE=Release ..
echo -e "${GREEN}Compiling Donnie for Lubuntu 14.04 ... ${NC}\n"
make
make install
echo -e "${GREEN}Donnie installed !!!! ${NC}\n"

echo -e "${GREEN}End of installation !!!! ${NC}\n"
