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

///function to get donnie host by environment variable
string GetEnv( const string & var );

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
  //RangerProxy *SHProxy;

  SpeechProxy *speech;

  int FrontBumper();
  int BackBumper();
	
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

  	void moveForward(float arg);
  	void moveBackward(float arg);
  	void turnRight(float arg);
  	void turnLeft(float arg);

	float GetRange(int arg);
	float GetPos(int arg);
	//float GetBumper(int arg);
	//void Scan();
	//void Status();

	/// call text-to-speech and print
	void speak(string text);
	
};
