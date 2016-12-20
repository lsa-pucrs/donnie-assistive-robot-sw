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
  double translationError;
  double rotation;
  double rotationError;
  int steps;
  int alertStepFlag;
  int alertBumperFlag;
  int alertRangerFlag;
  bool onMovement;

  string SSTEP;   //Sound STEP
  string SSBACK;  //Sound Step BACK
  string STRIGHT; //Sound Turn RIGHT
  string STLEFT;  //Sound Turn LEFT
  string SBUMPER; //Sound Turn LEFT
  string SRANGER; //Sound Turn LEFT


public:
  DonnieClient();
  pos_t getPos();
  void setPos(double x, double y, double a);
  void checkDir();
  void checkBumpers();
  void checkRangers();
};
