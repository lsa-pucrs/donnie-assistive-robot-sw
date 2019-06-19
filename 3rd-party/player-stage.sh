##################################################
# Install Player/Stage dependencies  
##################################################

echo -e "${GREEN}Installing Player/Stage Dependencies ... ${NC}\n"
sudo apt-get install -y libfltk1.1-dev 
sudo apt-get install -y freeglut3-dev 
sudo apt-get install -y libltdl-dev 
sudo apt-get install -y libjpeg-dev
case "${VER}" in 
	14.04)
		sudo apt-get install -y libpng12-dev 
		;;
	16.04)
		sudo apt-get install -y libpng12-dev 
		;;
	18.04)
		sudo apt-get install -y libpng-dev 
		;;
esac
# TODO. acho q essa lib nao eh necessaria pois nao precisei dela o p 18.04. Igor, testar
case "${VER}" in 
	14.04)
		sudo apt-get install -y libdb5.1-stl
		;;
	16.04)
		sudo apt-get install -y libdb5.3-stl
		;;
esac
sudo apt-get install -y libgnomecanvasmm-2.6-dev
#sudo apt-get install -y python-gnome2
#sudo apt-get install -y libboost-all-dev  # overkill, the actually required libraries are boostthread, boostsignal, boostsystem
sudo apt-get install -y libboost-signals-dev libboost-system-dev libboost-thread-dev
# old OpenCV for older Player drivers
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
# Compile and install Player/Stage 
##################################################
cd Player
set +e #Continue the script if there's already a patch applied
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
set -e
mkdir -p build # mkdir -p is safer !
cd build
# Mandatory
# DEBUG_LEVEL=NONE <==== important !!!
# Recommended: Build the Python bindings for the C client library
# BUILD_PYTHONCPP_BINDINGS:BOOL=ON
# BUILD_PYTHONC_BINDINGS:BOOL=ON
echo -e "${GREEN}Configuring Player ... ${NC}\n"
cmake -DCMAKE_BUILD_TYPE=Release -DDEBUG_LEVEL=NONE -BUILD_PYTHONC_BINDINGS:BOOL=OFF \
	-DBUILD_EXAMPLES:BOOL=OFF \
	-DENABLE_DRIVER_P2OS:BOOL=OFF \
	-DENABLE_DRIVER_ERRATIC:BOOL=OFF \
	-DENABLE_DRIVER_ROBOTEQ:BOOL=OFF \
	-DBUILD_CC_TESTS:BOOL=OFF \
..

echo -e "${GREEN}Compiling Player ... ${NC}\n"
make -j ${NUM_CORES} 
sudo make install
echo -e "${GREEN}Player installed !!!! ${NC}\n"

cd ../../Stage
mkdir -p build
cd build
# Mandatory
# CMAKE_BUILD_TYPE=release <==== important !!!
echo -e "${GREEN}Configuring Stage  ... ${NC}\n"
cmake -DCMAKE_BUILD_TYPE=Release ..
echo -e "${GREEN}Compiling Stage ... ${NC}\n"
make -j ${NUM_CORES}
sudo make install
echo -e "${GREEN}Stage installed !!!! ${NC}\n"
cd ../..

