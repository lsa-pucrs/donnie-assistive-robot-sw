#!/bin/bash
# TODO: This is a totally UNTESTED and BETA script for cross-compiling. 
# Use it if you have some experience with cross-compiling.
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
#
# Important directories:
# 	-cross compiling tools: /opt/raspberrypi/tools
# 	-RPi mount dir: /opt/raspberrypi/root

# TODO: future work, distributed compilation with distcc
# http://jeremy-nicola.info/portfolio-item/cross-compilation-distributed-compilation-for-the-raspberry-pi/
# TODO: create docker file with rpi cross compilation environment
# https://github.com/sdt/docker-raspberry-pi-cross-compiler

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


###########################################################
# set environment variables required for cross compilation
###########################################################
echo '# Donnie developer environment variables' >> ~/.bashrc
echo 'export RPI_ROOT_PATH=/opt/raspberrypi/root' >> ~/.bashrc
echo 'export RPI_TOOLS_PATH=/opt/raspberrypi/tools' >> ~/.bashrc
echo 'export RPI_IP=192.168.0.6' >> ~/.bashrc
echo '' >> ~/.bashrc
echo 'function mount_rpi {' >> ~/.bashrc
echo '        sudo sshfs pi@$RPI_IP:/ $RPI_ROOT_PATH -o transform_symlinks -o allow_other' >> ~/.bashrc
echo '}' >> ~/.bashrc
echo '' >> ~/.bashrc
echo 'function umount_rpi {' >> ~/.bashrc
echo '        sudo umount $RPI_ROOT_PATH' >> ~/.bashrc
echo '}' >> ~/.bashrc

export RPI_ROOT_PATH=/opt/raspberrypi/root
export RPI_TOOLS_PATH=/opt/raspberrypi/tools
export RPI_IP=192.168.0.6

export LD_LIBRARY_PATH=/usr/lib:/usr/local/lib/:$LD_LIBRARY_PATH
# Opencv lib path
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:$LD_LIBRARY_PATH
# Player lib path
export LD_LIBRARY_PATH=/usr/local/lib64/:$LD_LIBRARY_PATH

##################################################
# install cross compilation utils for RPi
##################################################
apt-get update
# required to mount RPi root dir
apt-get install -y sshfs

# One can build the tool chain (more labor) with crosstool-ng
#  https://blog.kitware.com/cross-compiling-for-raspberry-pi/
# or get pre-compile packages with 
#   sudo apt-get install g++-arm-linux-gnueabihf
#   sudo apt-get install gdb-multiarch
# However, since the official Raspberry Pi Toolchain is already available at github, 
# we use this approach

#https://solderspot.wordpress.com/2016/02/04/cross-compiling-for-raspberry-pi-part-ii/
mkdir -f /opt/raspberrypi
cd /opt/raspberrypi
echo -e "${GREEN}Installing Cross compilation utils for RPi ... ${NC}\n"
# The "--depth=1" is here to tell git we only want the last revision, and not the whole history to be cloned.
git clone https://github.com/raspberrypi/tools.git --depth=1 tools

##################################################
# mounting RPi disk on the host computer
##################################################
# how to mount using sshfs RPi disk on the host computer
# http://redmine.webtoolkit.eu/projects/wt/wiki/Cross_compile_Wt_on_Raspberry_Pi
# http://embedonix.com/articles/linux/emulating-raspberry-pi-on-linux/
# http://www.hertaville.com/cross-compiling-qt4-app.html
echo -e "${GREEN}Mounting RPi disk on the host computer ... ${NC}\n"
mkdir -f /opt/raspberrypi/root

sudo sshfs pi@$RPI_IP:/ $RPI_ROOT_PATH -o transform_symlinks -o allow_other
# required to have write permission into RPi dir
sudo usermod -a -G fuse lsa
sudo ln -s $RPI_ROOT_PATH/usr/lib/arm-linux-gnueabihf/ /usr/lib/arm-linux-gnueabihf
sudo ln -s $RPI_ROOT_PATH/lib/arm-linux-gnueabihf/ /lib/arm-linux-gnueabihf

##################################################
# Cross compile test application 
##################################################
mkdir install/cross/hello/build
cd install/cross/hello/build
cmake -DCMAKE_INSTALL_PREFIX=$RPI_ROOT_PATH/home/pi -DCMAKE_TOOLCHAIN_FILE=$DONNIE_PATH/rpi.cmake ..
make
make install
# go to RPi /~ and test this example

##################################################
# Compile and install Player
##################################################
#http://playerstage.sourceforge.net/wiki/Cross-compiling

cd Player
mkdir build-rpi
cd build-rpi
echo -e "${GREEN}Configuring Player for RPi ... ${NC}\n"
#disablingCMAKE_INSTALL_PREFIX all the unused drivers and utils...
cmake -DCMAKE_INSTALL_PREFIX=$RPI_ROOT_PATH/usr/local \
	-DPKG_CONFIG_EXECUTABLE=/home/lsa/Downloads/donnie-assistive-robot-sw/rpi-pkg-config  \
	-DCMAKE_TOOLCHAIN_FILE=$DONNIE_PATH/modules/rpi.cmake \
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
echo -e "${GREEN}Compiling Player for RPi ... ${NC}\n"
make
make install
echo -e "${GREEN}Player installed !!!! ${NC}\n"

##################################################
# cross compile and install OpenCV3
##################################################
# http://docs.opencv.org/2.4/doc/tutorials/introduction/crosscompilation/arm_crosscompile_with_cmake.html

##################################################
# cross compile raspicam driver
##################################################
#https://solderspot.wordpress.com/2016/02/04/cross-compiling-for-raspberry-pi-part-ii/
cd ../../raspicam
mkdir build-rpi
cd build-rpi
echo -e "${GREEN}Configuring raspicam for RPi ... ${NC}\n"
cmake -DCMAKE_INSTALL_PREFIX=$RPI_ROOT_PATH/usr/local -DCMAKE_TOOLCHAIN_FILE=${DONNIE_PATH}/modules/rpi.cmake \
	-DCMAKE_BUILD_TYPE=Release \
	..
echo -e "${GREEN}Compiling raspicam for RPi ... ${NC}\n"
make
make install
echo -e "${GREEN}Raspicam installed !!!! ${NC}\n"


##################################################
# cross compile and install Donnie
##################################################

cd ../../donnie-assistive-robot-sw
mkdir build-rpi
cd build-rpi
echo -e "${GREEN}Configuring Donnie for RPi ... ${NC}\n"
cmake -DCMAKE_TOOLCHAIN_FILE=${DONNIE_PATH}/modules/rpi.cmake \
	-DCMAKE_BUILD_TYPE=Release \
	..
echo -e "${GREEN}Compiling Donnie for RPi ... ${NC}\n"
make
make install
echo -e "${GREEN}Donnie installed !!!! ${NC}\n"


##################################################
# uninstall all dev packages to save space
##################################################
echo -e "${GREEN}Removing cache  ... ${NC}\n"
apt-get autoclean
apt-get autoremove

echo -e "${GREEN}End of installation !!!! ${NC}\n"


