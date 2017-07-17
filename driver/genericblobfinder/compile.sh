rm -f libGenericBlobfinder.so
mkdir -p build
cd build/
cmake ../
make
mv libGenericBlobfinder.so ../
