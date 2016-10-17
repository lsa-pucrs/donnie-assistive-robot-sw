#!/bin/bash
# TODO: This is a totally UNTESTED and BETA script for RPi emulating with qemu. 
# Use it if you have some experience with qemu.
#####################
# Author: Alexandre Amory
# Date: September/2016
# Laboratorio de Sistemas Autonomos - FACIN - PUCRS University
# Description:
#   This script cross compile all source codes to the RPi in the host computer.
#   No compilation is performed on RPi. 
#   Run this after installing the depedencies in the RPi (install-rpi.sh)
# How to Execute (host computer):
#
# Note that this script is meant for Donnie`s developers, not for users, 
# since one will execute this script only if the software running on RPi is changed.
# Normal users will burn a pre-build RPi image for their Donnie robot.
#
# go to Donnie source code dir
#   $ cd <donnie source>
# run this script
#   $ sudo ./install-cross-pi.sh

# Important directories:
# 	-RPi QEMU image dir: /opt/raspberrypi/qemu


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
	Ubuntu )
		echo -e "${ORANGE}WARNING:{NC} Ubuntu is only recommended for Destop computer, not VMs, and not Raspberry Pi\n"
		case ${VER} in 
			14.04 )
				echo -e "${GREEN}NOTE:{NC} ${OS} - ${VER} (${OSNAME} is the recommended OS version.\n"
				;;
			* )
				# Handle other OS versions here
				echo -e "${ORANGE}WARNING:{NC} ${OS} - ${VER} (${OSNAME} is not a recommended OS version. You might get errors and some programming experience is required to compile Donnie. \n"
				echo -e "${GREEN}NOTE:{NC} Ubuntu 14.04 (trusty) is the recommended version for ${OS}\n"
				exit 1;
			 ;;
		esac		
		;;
	Lubuntu )
		echo -e "${ORANGE}WARNING:{NC} Lubuntu is recommended for both Destop computers and VMs, but not for Raspberry Pi\n"
		case ${VER} in 
			14.04 )
				echo -e "${GREEN}NOTE:{NC} ${OS} - ${VER} (${OSNAME} is the recommended OS version.\n"
				;;
			* )
				# Handle other OS versions here
				echo -e "${ORANGE}WARNING:{NC} ${OS} - ${VER} (${OSNAME} is not a recommended OS version. You might get errors and some programming experience is required to compile Donnie. \n"
				echo -e "${GREEN}NOTE:{NC} Lubuntu 14.04 (trusty) is the recommended version for ${OS}\n"
				exit 1;
			 ;;
		esac			
	* )
     # Handle other distributions here
		echo -e "${RED}ERROR:{NC} ${OS} - ${VER} (${OSNAME} is not a supported OS\n"
		echo -e "${GREEN}NOTE:{NC}Lubuntu 14.04 (trusty) is recommended OS for Donnie's host computer\n"
		exit 1;
     ;;
esac


##################################################
# install cross compilation utils 
##################################################
apt-get update

#http://hackaday.com/2016/02/03/code-craft-cross-compiling-for-the-raspberry-pi/
#sudo apt-get install g++-arm-linux-gnueabihf
sudo apt-get install gdb-multiarch

# download rasp toolchain and create QEMU virtual RPI
#http://blog.julusian.co.uk/raspberry-pi/2015/05/22/pi-cross-compile.html
apt-get install  gcc-multilib debootstrap qemu-user-static
sudo debootstrap --foreign --arch armhf jessie . http://archive.raspbian.org/raspbian

http://redmine.webtoolkit.eu/projects/wt/wiki/Cross_compile_Wt_on_Raspberry_Pi
https://cmake.org/Wiki/CMake_Cross_Compiling
http://playerstage.sourceforge.net/wiki/Cross-compiling

- na RPI, instale todas as dependencias de pacotes c apt-get install
- no host, instalar cross compilation tool chain da RPI
- monte com sshfs o  da RPI no host.
- fazer cmake toolchain file for cross compilation



# nao testado http://embedonix.com/articles/linux/emulating-raspberry-pi-on-linux/
# use sshfs to mount / of RPI into the host computer
http://redmine.webtoolkit.eu/projects/wt/wiki/Cross_compile_Wt_on_Raspberry_Pi
http://www.hertaville.com/cross-compiling-qt4-app.html
sudo sshfs pi@192.168.0.104:/ /home/halherta/rpi/mntrpi/ -o transform_symlinks -o allow_other
sudo ln -s $HOME/rpi/mntrpi/usr/lib/arm-linux-gnueabihf/ /usr/lib/arm-linux-gnueabihf
sudo ln -s $HOME/rpi/mntrpi/lib/arm-linux-gnueabihf/ /lib/arm-linux-gnueabihf

# nice to have, not mandatory
apt-get install -y geany

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
# TODO correct the path for RPI
# run 'sudo find / -name "*.pc" -type f' to find all the pc files for pkg-config
echo 'export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib64/pkgconfig/:/usr/lib/pkgconfig/:/usr/lib/x86_64-linux-gnu/pkgconfig/:/usr/share/pkgconfig/' >> ~/.bashrc
# run 'sudo find / -name "*.cmake" -type f' to find all the pc files for pkg-config
echo 'export CMAKE_MODULE_PATH=$CMAKE_MODULE_PATH:/usr/local/lib64/cmake/Stage/:/usr/local/share/cmake/Modules/:/usr/share/cmake-2.8/Modules/:/usr/share/OpenCV/' >> ~/.bashrc


export LD_LIBRARY_PATH=/usr/lib:/usr/local/lib/:$LD_LIBRARY_PATH
# Opencv lib path
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:$LD_LIBRARY_PATH
# Player lib path
export LD_LIBRARY_PATH=/usr/local/lib64/:$LD_LIBRARY_PATH


##################################################
# Compile and install Player
##################################################

cd Player
#echo -e "${GREEN}Patching Player for RPi ... ${NC}\n"
#patch -p1 < patch/festival/festival.patch
#patch -p1 < patch/install/player_3.0.2_14.04.patch
#echo -e "${GREEN}Patching Player for Donnie ... ${NC}\n"
#patch -p1 < ../donnie-assistive-robot-sw/proxies/instalationSoundProxy.patch
mkdir build-rpi
cd build-rpi
echo -e "${GREEN}Configuring Player for RPi ... ${NC}\n"
#disabling all the unused drivers and utils...
cmake -DCMAKE_BUILD_TYPE=Release \
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
	.. 
echo -e "${GREEN}Compiling Player for RPi ... ${NC}\n"
make
make install
echo -e "${GREEN}Player installed !!!! ${NC}\n"


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
#sudo apt-get install -y libreadline-dev
#sudo apt-get install -y oracle-java8-installer
#sudo apt-get install -y libantlr3c-dev
#to compile docs
#sudo apt-get install -y texlive-full
#sudo apt-get install -y doxygen

cd ../../donnie-assistive-robot-sw
mkdir build
cd build
echo -e "${GREEN}Configuring Donnie for Lubuntu 14.04 ... ${NC}\n"
cmake -DCMAKE_BUILD_TYPE=Release ..
echo -e "${GREEN}Compiling Donnie for Lubuntu 14.04 ... ${NC}\n"
make
make install
echo -e "${GREEN}Donnie installed !!!! ${NC}\n"


##################################################
# uninstall all dev packages to save space
##################################################
echo -e "${GREEN}Removing compilation tools ... ${NC}\n"
apt-get purge -y build-essential
apt-get purge -y autoconf
apt-get purge -y cmake
apt-get purge -y cmake-curses-gui
apt-get purge -y git
apt-get purge -y pkg-config


echo -e "${GREEN}End of installation !!!! ${NC}\n"


