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
#sudo apt-get install -y libboost-all-dev  # overkill, the actually required libraries are boostthread, boostsignal, boostsystem
sudo apt-get install -y libboost-signals-dev libboost-system-dev libboost-thread-dev
# The Player drivers were compiled with opencv 3.1, so, do not install the default opencv because it is 2.4
#sudo apt-get install -y libopencv-dev libopencv-core-dev libcv-dev libcvaux-dev libhighgui-dev
# alsa - sound player
# http://player-stage-gazebo.10965.n7.nabble.com/CCmake-cannot-find-the-existing-asoundlib-h-for-ALSA-driver-td11198.html
sudo apt-get install -y libasound2-dev
# alsa alsa-tools  alsa-utils
# for pmap
sudo apt-get install -y libgsl0-dev libxmu-dev
# for python bindings for Player clients - 
# It is not recommended to use python due to limitations in the bindings. 
# Things that work on a C/C++ client might not work on a Python client.
#sudo apt-get install -y python-dev swig
# PostGIS for a Player driver
sudo apt-get install -y libpq-dev libpqxx-dev

##################################################
# Compiling Player 
##################################################

cd Player
patch -p1 -N --dry-run --silent < patch/festival/festival.patch 2>/dev/null
#If the patch has not been applied then the $? which is the exit status 
#for last command would have a success status code = 0
if [ $? -eq 0 ];
then
    #apply the patch
	echo -e "${GREEN}Patching Player ... ${NC}\n"
	patch -p1 < patch/festival/festival.patch
	patch -p1 < patch/install/player_3.0.2_14.04.patch
	patch -p1 < patch/donnie/instalationSoundProxy.patch
fi
mkdir -p build # mkdir -p is safer !
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
	-DENABLE_DRIVER_SHAPETRACKER:BOOL=OFF \
	-DENABLE_DRIVER_SICKLDMRS:BOOL=OFF \
	-DENABLE_DRIVER_SICKLMS200:BOOL=OFF \
	-DENABLE_DRIVER_SICKLMS400:BOOL=OFF \
	-DENABLE_DRIVER_SICKNAV200:BOOL=OFF \
	-DENABLE_DRIVER_SICKRFI341:BOOL=OFF \
	-DENABLE_DRIVER_SICKS3000:BOOL=OFF \
	-DENABLE_DRIVER_SKYETEKM1:BOOL=OFF \
	-DENABLE_DRIVER_SONYEVID30:BOOL=OFF \
	-DENABLE_DRIVER_SPHEREPTZ:BOOL=OFF \
	-DENABLE_DRIVER_STATGRAB:BOOL=OFF \
	-DENABLE_DRIVER_SUPPRESSOR:BOOL=OFF \
	-DENABLE_DRIVER_UPCBARCODE:BOOL=OFF \
	-DENABLE_DRIVER_WBR914:BOOL=OFF \
	-DPLAYER_BUILD_TESTS:BOOL=OFF \
	-DBUILD_CC_TESTS:BOOL=OFF \
	.. 
echo -e "${GREEN}Compiling Player ... ${NC}\n"
make -j ${NUM_CORES} 
sudo make install
echo -e "${GREEN}Player installed !!!! ${NC}\n"
cd ../..

