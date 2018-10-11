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

class vib_belt {
//private:
public:
    char devicename[80], ch;
    int fd;
    int status;
    string host;
    int port;
    PlayerClient *robot;
    RangerProxy *myranger;
    char P[36];
    // The arduino protocol for controlling the motors is 'z' to indicate de start
    // of the information, 'A' for maximum vibration, 'B' for moderate and 'C' for low
public:
    void signal_handler_IO(int);

    vib_belt(const char* = NULL, const char* = NULL, int = 0);

	char* getP();

    bool connect();
    bool loop();
    void exit();
};

#endif
