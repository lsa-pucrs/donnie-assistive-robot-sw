##################################################
# install an optiomized openCV 3.2 for Rasp 3
# https://www.pyimagesearch.com/2017/10/09/optimizing-opencv-on-the-raspberry-pi/
# replace the opencv version to https://github.com/opencv/opencv/archive/3.2.0.zip
# opencv is not included as a submodule because it is too big, delaying the 'recursive clone' operation
# pre-built packages https://yoursunny.com/t/2018/install-OpenCV3-PiZero/
##################################################
# OpenCV's depedencies

echo -e "${GREEN}Installing OpenCV Dependencies ... ${NC}\n"
echo 'deb [trusted=yes] http://dl.bintray.com/yoursunny/PiZero stretch-backports main' |  sudo tee  /etc/apt/sources.list.d/bintray-yoursunny-PiZero.list
sudo apt update

echo -e "${GREEN}Instaling OpenCV ... ${NC}\n"
sudo apt install -y libopencv-dev
#sudo apt install -y python3-opencv

echo -e "${GREEN}OpenCV Installed !!! ${NC}\n"
