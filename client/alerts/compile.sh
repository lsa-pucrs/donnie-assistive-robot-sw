rm -f alerts # no error if not existing
mkdir -p build # no error if existing
cd build/
cmake ../
make
mv alerts ../
