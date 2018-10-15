if [[ ! -e build ]]; then
    mkdir build
	echo "build directory created for compilation!" 
fi
cd build/
cmake ../
make
sudo make install