rm -f libsoxplayer.so
mkdir -p build
cd build/
cmake ../
make
mv libsoxplayer.so ../
