#ifndef VIB_BELT_H
#define VIB_BELT_H

#include <libplayerc++/playerc++.h>
#include <iostream>
#include <cstring>
#include <string>

using namespace std;
using namespace PlayerCc;

// distance in meters (to be confirmed!!!)
#define INTENSE_VIBRATION_DISTANCE 1.0 // distances lower than this will cause intense vibration
#define MILD_VIBRATION_DISTANCE 1.5    // distances between INTENSE_VIBRATION_DISTANCE and MILD_VIBRATION_DISTANCE will cause mild vibration
// distances above MILD_VIBRATION_DISTANCE will cause no vibration

class vib_belt {
private:
    // host ip
    string host;
    // host port number
    int port;
    PlayerClient *robot;
    RangerProxy *ranger;
    SpeechProxy *speech;
    // the content sent to the serial port
    char P[36];
    // The arduino protocol for controlling the motors is 'z' to indicate de start
    // of the information, 'A' for maximum vibration, 'B' for moderate and 'C' for low
public:

    vib_belt(const char* = NULL, int = 0);
    
    //read the rangers and update the vib motors
    bool loop();
   
};

#endif
