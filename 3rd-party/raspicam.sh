#https://solderspot.wordpress.com/2016/02/04/cross-compiling-for-raspberry-pi-part-ii/
cd raspicam
mkdir -p build
cd build
echo -e "${GREEN}Configuring raspicam ... ${NC}\n"
cmake -DCMAKE_BUILD_TYPE=Release \
	..
echo -e "${GREEN}Compiling raspicam ... ${NC}\n"
make -j ${NUM_CORES}
sudo make install
echo -e "${GREEN}Raspicam installed !!!! ${NC}\n"

#run the follwing command to test raspicam
#raspivid -o - -t 9999999 -w 1280 -h 1024 -b 500000 -fps 20 -vf

#Testing Raspicam via ssh
#https://www.raspberrypi.org/forums/viewtopic.php?t=67571
#http://raspi.tv/2013/how-to-stream-video-from-your-raspicam-to-your-nexus-7-tablet-using-vlc
#1) Precisa do vlc (para o comando cvlc)
sudo apt-get install -y vlc
#2) Executar processo da camera
#raspivid -o - -t 99999 -w 640 -h 360 -fps 5 -vf|cvlc -vvv stream:///dev/stdin --sout '#standard{access=http,mux=ts,dst=:8090}' :demux=h264
#Obs: Para conectar entrar no endereço (pelo streaming do VLC no PC) http://192.168.0.XX:8090 