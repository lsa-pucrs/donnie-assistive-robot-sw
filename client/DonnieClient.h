#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cmath>
#include "utils.h"
#include <libplayerc++/playerc++.h>

using namespace std;
using namespace PlayerCc;


struct PathNodes
{
  double posx, posy;
};



class DonnieClient
{

  PlayerClient *robot;
  PlayerClient *head;

  Position2dProxy *p2dProxy;
  Position2dProxy *p2d_headProxy;

  BumperProxy *bpProxy;

  BlobfinderProxy *BfinderProxy;

  RangerProxy *sonarProxy;
  RangerProxy *SHProxy;

  SpeechProxy *speech;

  int FrontBumper();
  int BackBumper();

public:
	DonnieClient();

  	void ParaFrente(float arg);
  	void ParaTras(float arg);
  	void ParaDireita(float arg);
  	void ParaEsquerda(float arg);

  float GetRange(int arg);
  float GetPos(int arg);
  //float GetBumper(int arg);

  //void Scan();

  //void Historico();
  //void Status();

  //void Falar(void* data, int arg);

  //void Sair();

};