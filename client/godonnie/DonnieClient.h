/*
 * Desc: This class implements the GoDonnie commands via Player middleware
 * Author: Augusto Bergamin, Alexandre Amory
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
#include <fstream>
#include <sstream>
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
  Position2dProxy *p2dProxy;
  Position2dProxy *p2d_headProxy;
  BumperProxy *bpProxy;
  BlobfinderProxy *bfinderProxy;
  RangerProxy *sonarProxy;
  RangerProxy *headSonarProxy;
  SpeechProxy *speechProxy;
  
  //! if on, it wont play any sound and it works like a printf
  bool muted;

  int FrontBumper();
  int BackBumper();
  float GetPos(Position2dProxy *p2d,int arg);

	//!Rotação da cabeça relativo a base do robo 
	/*! A posicao que este goto tenta atingir depende da posicao da base. 
		Exemplo, Se mandar a cabeca do robo ir para 45 graus(em relacao ao robo) e a base estiver em 10 graus(em relacao ao mundo) 
		a cabeca vai para  55 graus (em relacao ao mundo).
		/param pa em graus
		/return 0 se chegou no destino sem imprevistos
		/return 1 se atingiu tempo limite TODO ou bateu TODO
	*/	
	int headGoto(float pa);

  	//! Rotates [in degree] the robot base
  	/*! make goto return the actual movement, considering a colision*/
  	int Goto(float pa); //rotation only [degrees]
	
	//Singleton
	DonnieClient();
	~DonnieClient();
	// instance pointer
	static DonnieClient *singleton;

	//to do: set the language!
	string language;
	//MOD DANIEL
	//! Returns a string with the name of the color detected by the blobfinder. Uses files "rgb-pt-br.txt" and "rgb-en.txt" in the current version.
	/*!
		/param value identifier of the color detected by the blobfinder. 
		/return string containing the name of the color. If the color was not identified, returns "unknown"(english) or "desconhecido"(portuguese), defined by  "LANG".
	*/
	string value_to_color(int color_value);
	
	//MOD DANIEL
	//! Returns the value of a known color (format: 0x00RRGGBB)
	/*!
		/param input_color name of the color in "eng" (english) or "pt-br" (brazilian portuguese), defined by "LANG".
		/return number of the color in the format 0x00RRGGBB. If the color is unknown, return 0xFFFFFFFF.
	*/
	int color_to_value(string input_color);
	
public:
	// create instante of singleton
	static DonnieClient *getInstance();
	// delete singleton
	static void ResetInstance();

	//! move fd without using Goto
  	int moveForward(float arg);

	//! move fd without using Goto
  	int moveBackward(float arg);

	//! run Goto and run TTS
  	int GotoTTS(float pa);

	//! turn TTS on/off
  	void muteTTS(bool m);
  	
	//! please describe me
	float GetRange(int arg);

	//! please describe me
	float GetPos(string p2d,int arg);
	
	//! scan 180 degree for obstacle using the sonar.
	/*! ranges is writen with sonar readings every 30 degree.
	    180/30 = 7 sonar readings. 0o, 30o, 60o, 90o, 120o, 150o, 180o.
	*/ 
	void Scan(float *sonar_readings, int *blobs_found);
	
	//! returns the number of times the color was found around the robot. 
	/*! color is encodedd in 0x00RRGGBB format */
	int Color(int color);
	
	//void Status();

	//! returns true when donnie bumped during the movements
	int bumped();

	//! call text-to-speech or print, depending if it is muted or not
	void speak(string text);




	
};

#endif
