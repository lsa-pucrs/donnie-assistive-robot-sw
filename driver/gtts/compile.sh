rm -f libgtts.so
mkdir -p build
cd build/
cmake ../
make
mv libgtts.so ../
