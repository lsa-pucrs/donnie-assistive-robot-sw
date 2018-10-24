//marques lsa
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cmath>
#include "utils.h"
#include <libplayerc++/playerc++.h>

// Localization libraries 
#include <boost/locale.hpp>

//yaml-cpp
#include <fstream>
#include "yaml-cpp/yaml.h"

using namespace std;
using namespace PlayerCc;
using namespace boost::locale;
using boost::locale::translate;
using boost::locale::format;

class FloorClient{
  PlayerClient *robot;
  Position2dProxy *p2d;
  SpeechProxy *speech;
  
  ifstream fin;
  YAML::Parser parser;
  YAML::Node doc;

  void getPos();
  void up();
  void down();
  void child();
  void parent();
  
public:
  FloorClient();
  void checkArrowKeys();
};
