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

struct PosXY{
	float x,y;
};

struct Room{
   string name;
   string description;
   PosXY bl_pos;
   PosXY tr_pos;
   float area;
   vector<string> objects;
   vector<PosXY> doors;
   // TODO preciso fazer um destrutor p os vectors ?
};

struct Floorplan{
   string name;
   string description;
   PosXY bl_pos;
   PosXY tr_pos;
   float area;
   vector<Room> rooms;
   // TODO preciso fazer um destrutor p os vectors ?
};

// cout operator for debug purposes
std::ostream& operator<<(std::ostream& os, const PosXY& m);
std::ostream& operator<<(std::ostream& os, const Room& m);
std::ostream& operator<<(std::ostream& os, const Floorplan& m);

// extraction of data from the YAML files
void operator >> (const YAML::Node& node, PosXY& v);
void operator >> (const YAML::Node& node, Room& r);
void operator >> (const YAML::Node& node, Floorplan& r);

class FloorClient{
  PlayerClient *robot;
  Position2dProxy *p2d;
  SpeechProxy *speech;
  
  ifstream fin;
  YAML::Parser parser;
  YAML::Node doc;
  Floorplan floorplan;
  PosXY  current_robot_pos;

  void getPos();
  void up();
  void down();
  void child();
  void parent();
  
public:
  FloorClient();
  void checkKeys();
};
