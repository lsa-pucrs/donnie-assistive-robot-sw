#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cmath>
#include "utils.h"
#include <libplayerc++/playerc++.h>

//This file implements the GoDonnie commands


using namespace std;
//namespace for use with the player library
using namespace PlayerCc;

//function to get donnie host by environment variable
string GetEnv( const string & var );

struct PathNodes
{
  double posx, posy;
};

//Class to Keep the Commands History
class Historic
{	
	private:
		//Struct to save commands, no external access alowed
		struct Command {
			string name;
			string feedback;
			double posx;
			double posy;
			double degree;
		};
		list<Command> commandsList;
		//Bidiretional Iterator
		list<Command>::iterator iterator;
		bool resetIterator();
		
		//Variable to hold Singleton 
		static Historic singleton;
		
		//Block the construction and destruction of the object
		Historic();
		~Historic();
			
	public:
		//Method to get the Singleton
		static Historic& getHistoric();
		
		
		bool addCommand(string name, string feedback);
		int size();
		string getLast();
		string getLine(int line);
		string previous();
		string next();
};


//Class to comunicate with Player Proxy
//Work in progress to turn in a Singleton
//The default Player library proxies can be found at http://playerstage.sourceforge.net/doc/Player-2.0.0/player/classPlayerCc_1_1ClientProxy.html
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
	static DonnieClient singleton;
	
public:
	static DonnieClient& getInstance();

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

	//Que gambiarra feia esse negocio de void*
	//Mudar para Overload
	//void Falar(void* data, int arg);
	bool Falar(int data, int arg);
	bool Falar(string text, int arg);
	

  //void Sair();

};
