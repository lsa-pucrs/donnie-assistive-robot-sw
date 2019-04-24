rm -f client-test # no error if not existing
mkdir -p build # no error if existing
cd build/
cmake ../
make
mv soxplayer-test ../
