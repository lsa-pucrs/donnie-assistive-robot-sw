rm -f client_donnie # no error if not existing
mkdir -p build # no error if existing
cd build/
cmake ../
make
#mv client_donnie ../
#TODO colocar regras make install no cMAKE. revisar p q todos drivers tenham isso.
