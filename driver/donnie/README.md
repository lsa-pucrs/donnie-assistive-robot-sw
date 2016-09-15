#### To compile this driver ####
.1. Make a directory called "build" and compile the driver ".
.2. Compile the driver with:
```bash  
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```
  
#### Upload firmware to Arduino (on PC)
Obs: This firmware needs Arduino IDE 1.2 or later.
Obs2: "/dev/ttyACM0" is the common default port, but it can be "..ACM1", "..ACM2", etc.
Obs3: If you receive an "port permission denied" error then follow the "automatic chmod for arduino port" steps before upload the first time.
.1. Open the "donnie/firmware.ino" on Arduino IDE.
.2. Chose the Arduino mega Board in "Tools>Board" tab.
.3. Chose the "/dev/ttyACM0" Port in "Tools>Port" tab. 
.4. Click on the upload Button

#### Automatic chmod for arduino port (only if you can't upload code)
.1. Type the following commands.
```bash  
$ sudo su
$ echo "ACTION=="add", ATTRS{idProduct}=="0043", ATTRS{idVendor}=="2341", DRIVERS=="usb", RUN+="chmod a+rw /dev/ttyACM0", SYMLINK+="arduino_UNO"" >> /etc/udev/rules.d/50-arduino_uno.rules
$ exit
```

#### To run this example:####

.1. In one terminal (In the Raspberry):
```bash
$ cd ~/player-3.0.2/examples/plugins/donnie
$ player server_donnie.cfg
```

.2. In another terminal (In the PC):
 ```bash
$ cd ~/player-3.0.2/examples/plugins/donnie/client
$ make
$ make run 
```

#### If you are using another serial port: ####
.1. Change it editing the port in "server_donnie.cfg" file:
```bash
$ nano server_donnie.cfg
```
And change the port, default port is "/dev/ttyAMA0".
Ctrl+X to exit nano, Y to save and Enter to confirmate the name.


