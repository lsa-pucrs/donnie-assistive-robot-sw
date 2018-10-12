#ifndef VIB_BELT_H
#define VIB_BELT_H

#include <libplayerc++/playerc++.h>
#include <iostream>
#include <cstring>
#include <string>

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <sys/types.h>
#define BAUDRATE B9600

using namespace std;
using namespace PlayerCc;

// distance in meters (to be confirmed!!!)
#define INTENSE_VIBRATION_DISTANCE 1.0 // distances lower than this will cause intense vibration
#define MILD_VIBRATION_DISTANCE 1.5    // distances between INTENSE_VIBRATION_DISTANCE and MILD_VIBRATION_DISTANCE will cause mild vibration
// distances above MILD_VIBRATION_DISTANCE will cause no vibration

class vib_belt {
public:
	// serial port name
    char devicename[80];
    // serial port file descriptor
    int fd;
    // 1 when serial interface has been connected
    int status;
    // host ip
    string host;
    // host port number
    int port;
    PlayerClient *robot;
    RangerProxy *myranger;
    // the content sent to the serial port
    char P[36];
    // The arduino protocol for controlling the motors is 'z' to indicate de start
    // of the information, 'A' for maximum vibration, 'B' for moderate and 'C' for low
public:
    void signal_handler_IO(int);

    vib_belt(const char* = NULL, const char* = NULL, int = 0);
	
	// print the content sent to the serial port for debug purposes
	void print_serial();

	// connects to the serial port
    bool connect();
    
    //read the rangers and update the vib motors
    bool loop();
    
    // close the serial port
    void exit();
};

#endif
