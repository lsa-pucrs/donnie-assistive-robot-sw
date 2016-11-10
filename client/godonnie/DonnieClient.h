/*
 * Desc: This class implements the GoDonnie commands via Player middleware
 * Author: Augusto Bergamin
 * Date:  November 2016
 * Laboratório de Sistemas Autônomos 
 *  - https://lsa.pucrs.br/
 *  - https://github.com/lsa-pucrs
 * Faculdade de Informática - PUCRS  
 *  - www.inf.pucrs.br
 */

#pragma once
#ifndef DONNIECLIENT_H
#define DONNIECLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cmath>
#include "utils.h"
#include <libplayerc++/playerc++.h>

using namespace std;
//namespace for use with the player library
using namespace PlayerCc;

struct PathNodes
{
  double posx, posy;
};

/**Class to comunicate with Player Proxy
The default Player library proxies can be found at http://playerstage.sourceforge.net/doc/Player-2.0.0/player/classPlayerCc_1_1ClientProxy.html
*/
class DonnieClient
{
private:
  PlayerClient *robot;
  //PlayerClient *head;
  Position2dProxy *p2dProxy;
  Position2dProxy *p2d_headProxy;
  //ActArrayProxy *actuator;
  BumperProxy *bpProxy;
  //BlobfinderProxy *BfinderProxy;
  RangerProxy *sonarProxy;
  SpeechProxy *speechProxy;

  int FrontBumper();
  int BackBumper();
  void turnRight(Position2dProxy *p2d,float arg);
  void turnLeft(Position2dProxy *p2d,float arg);
  float GetPos(Position2dProxy *p2d,int arg);

	//Singleton
	DonnieClient();
	~DonnieClient();
	// instance pointer
	static DonnieClient *singleton;
	
public:
	// create instante of singleton
	static DonnieClient *getInstance();
	// delete singleton
	static void ResetInstance();

	/// 
  	int moveForward(float arg);
  	int moveBackward(float arg);
  	void turnRight(string p2d,float arg);
  	void turnLeft(string p2d,float arg);
  	//void gotoYawHead(float arg);

	float GetRange(int arg);
	float GetPos(string p2d,int arg);
	//float GetBumper(int arg);
	
	/// scan 180 degree for obstacle using the sonar.
	/// ranges is writen with sonar readings every 30 degree.
	/// 180/30 = 7 sonar readings. 0o, 30o, 60o, 90o, 120o, 150o, 180o.
	void Scan(float *sonar_readings);
	//void Status();
	
	/// returns true when donnie bumped during the movements
	int bumped();

	/// call text-to-speech and print
	void speak(string text);
	
};

#endif
