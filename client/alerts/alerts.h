//marques lsa
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cmath>
#include "utils.h"
#include <libplayerc++/playerc++.h>

using namespace std;
using namespace PlayerCc;

//function to get donnie host by environment variable
string GetEnv( const string & var );
typedef struct PathNodes{
  double x, y, a;
} pos_t;


class DonnieClient{
  PlayerClient *robot;
  //PlayerClient *head;
  Position2dProxy *p2d;
  Position2dProxy *p2dhead;
  //ActArrayProxy *actuator;
  BumperProxy *bumper;
  //BlobfinderProxy *BfinderProxy;
  RangerProxy *ranger;
  //RangerProxy *SHProxy;
  SpeechProxy *speech;
  SoundProxy *sound;
  pos_t pos;
  double translation;
  double rotation;
  int steps;
  int alertStepFlag;
  int alertBumperFlag;
  int alertRangerFlag;


public:
  DonnieClient();
  pos_t getPos();
  void setPos(double x, double y, double a);
  void checkDir();
  void checkBumpers();
  void checkRangers();
};