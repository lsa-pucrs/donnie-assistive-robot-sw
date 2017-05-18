/*
* Desc: Donnie's Player-based driver test application.
* Author: Guilherme Marques - 
* Date:  August 2016
* Laboratório de Sistemas Autônomos 
*  - https://lsa.pucrs.br/
*  - https://github.com/lsa-pucrs
* Faculdade de Informática - PUCRS  
*  - www.inf.pucrs.br
*/

//TODO: Marques, colocar um breve manual de como usar esta aplicacao. pode ser em portugues
// dar uma comentada e limpada no codigo

#include <strings.h>
#include <iostream>
#include <termios.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* for strcpy() */
#include <stdlib.h>  /* for atoi(3),rand(3) */
#include <time.h>
#include <pthread.h>
#include <libplayerc++/playerc++.h>
#include <cmath>

//Parameters
#define LINEAR_DEFAULT_VEL 0.04    //[m/s]
#define FRONT_RANGER_STALL_SETPOINT 0.15  //[m]

#define KEYCODE_A 0x61
#define KEYCODE_B 0x62
#define KEYCODE_C 0x63
#define KEYCODE_D 0x64
#define KEYCODE_E 0x65
#define KEYCODE_I 0x69
#define KEYCODE_J 0x6a
#define KEYCODE_K 0x6b
#define KEYCODE_L 0x6c
#define KEYCODE_M 0x6d
#define KEYCODE_O 0x6F
#define KEYCODE_Q 0x71
#define KEYCODE_R 0x72
#define KEYCODE_S 0x73
#define KEYCODE_T 0x74
#define KEYCODE_U 0x75
#define KEYCODE_V 0x76
#define KEYCODE_W 0x77
#define KEYCODE_X 0x78
#define KEYCODE_Z 0x7a

#define KEYCODE_AA 0x41
#define KEYCODE_BB 0x42
#define KEYCODE_CC 0x43
#define KEYCODE_DD 0x44
#define KEYCODE_EE 0x45
#define KEYCODE_FF 0x46
#define KEYCODE_GG 0x47


#define KEYCODE_COMMA 0x2c
#define KEYCODE_PERIOD 0x2e

using namespace PlayerCc;
using namespace std;


// should we continuously send commands?
bool always_command = false;

// are we in debug mode (dont send commands)?
bool debug_mode = false;

// do we print out speeds?
bool print_speeds = false;

// send carlike commands
bool use_car = false;

char lastKey = 0;

// define the speed limits for the robot

// at full joystick depression you'll go this fast
double max_speed = 0.500; // m/second
double max_turn = DTOR(60); // rad/second
char jsdev[PATH_MAX]; // joystick device file

// this is the speed that the camera will pan when you press the
// hatswitches in degrees/sec
/*
#define PAN_SPEED 2
#define TILT_SPEED 2
*/


struct controller
{
  double speed, turnrate;
  bool dirty; // use this flag to determine when we need to send commands
};

//variables to get the console in raw mode
int kfd = 0;
struct termios cooked, raw;

// read commands from the keyboard
void* keyboard_handler(void* arg)
{
  char c;
  double max_tv = max_speed;
  double max_rv = max_turn;

  int speed=0;
  int turn=0;

  // cast to a recognized type
  struct controller* cont = (struct controller*)arg;

  // get the console in raw mode
  tcgetattr(kfd, &cooked);
  memcpy(&raw, &cooked, sizeof(struct termios));
  raw.c_lflag &=~ (ICANON | ECHO);
  raw.c_cc[VEOL] = 1;
  raw.c_cc[VEOF] = 2;
  tcsetattr(kfd, TCSANOW, &raw);

  for(;;)
  {
	// get the next event from the keyboard
	if(read(kfd, &c, 1) < 0)
	{
	  perror("read():");
	  exit(-1);
	}

	switch(c)
	{
	  case KEYCODE_I:
		speed = 1;
		turn = 0;
		cont->dirty = true;
		lastKey = 'i';
		//puts("i");
		break;
	  case KEYCODE_K:
		speed = 0;
		turn = 0;
		cont->dirty = true;
		lastKey = 'k';
		//puts("k");
		break;
	  case KEYCODE_O:
		speed = 1;
		turn = -1;
		cont->dirty = true;
		break;
	  case KEYCODE_J:
		speed = 0;
		turn = 1;
		cont->dirty = true;
		lastKey = 'j';
		//puts("j");
		break;
	  case KEYCODE_L:
		speed = 0;
		turn = -1;
		cont->dirty = true;
		lastKey = 'l';
		//puts("l");
		break;
	  case KEYCODE_U:
		turn = 1;
		speed = 1;
		cont->dirty = true;
		break;
	  case KEYCODE_COMMA:
		turn = 0;
		speed = -1;
		cont->dirty = true;
		lastKey = ',';
		break;
	  case KEYCODE_PERIOD:
		turn = 1;
		speed = -1;
		cont->dirty = true;
		break;
	  case KEYCODE_M:
		turn = -1;
		speed = -1;
		cont->dirty = true;
		break;
	  case KEYCODE_Q:
		max_tv += max_tv / 10.0;
		max_rv += max_rv / 10.0;
		if(always_command)
		  cont->dirty = true;
		break;
	  case KEYCODE_Z:
		max_tv -= max_tv / 10.0;
		max_rv -= max_rv / 10.0;
		if(always_command)
		  cont->dirty = true;
		break;
	  case KEYCODE_W:
		max_tv += max_tv / 10.0;
		if(always_command)
		  cont->dirty = true;
		break;
	  case KEYCODE_X:
		max_tv -= max_tv / 10.0;
		if(always_command)
		  cont->dirty = true;
		break;
	  case KEYCODE_E:
		max_rv += max_rv / 10.0;
		if(always_command)
		  cont->dirty = true;
		break;
	  case KEYCODE_C:
	  	lastKey = 'c';
		max_rv -= max_rv / 10.0;
		if(always_command)
		  cont->dirty = true;
		break;
	  case KEYCODE_R:
		break;
	  case KEYCODE_S:
	  	lastKey = 's';
		max_rv -= max_rv / 10.0;
		if(always_command)
		  cont->dirty = true;
		break;
	  case KEYCODE_V:
		break;
	  case KEYCODE_B:
		break;
	  case KEYCODE_T:
		break;
	  case KEYCODE_AA:
	  	lastKey = 'A';
	  	cont->dirty = true;
		break;
	  case KEYCODE_BB:
	  	lastKey = 'B';
	  	cont->dirty = true;
		break;
	  case KEYCODE_CC:
	  	lastKey = 'C';
	  	cont->dirty = true;
		break;
	  case KEYCODE_DD:
	  	lastKey = 'D';
	  	cont->dirty = true;
		break;
	  case KEYCODE_EE:
	  	lastKey = 'E';
	  	cont->dirty = true;
		break;
	  case KEYCODE_FF:
	  	lastKey = 'F';
	  	cont->dirty = true;
		break;
	  case KEYCODE_GG:
	  	lastKey = 'G';
	  	cont->dirty = true;
		break;
	  default:
		speed = 0;
		turn = 0;
		cont->dirty = false;
	}
	if (cont->dirty == true)
	{
	  cont->speed = speed; // * max_tv;
	  cont->turnrate = turn; // * max_rv;
	}
  }
  return(NULL);
}



unsigned int neckServoPos = 90; 

/*
void foward(){
	/*mymotors.SetSpeed(0.04,0);
	int aux = atoi(cmdValue.c_str());

				int atualXPos = mymotors.GetXPos();
				lastXPos = atualXPos;
				while(true){
					robot.Read();
					atualXPos = mymotors.GetXPos();
					if(option.at(1)=='f'       && (atualXPos-lastXPos)>=aux) break;
					else if (option.at(1)=='t' && (lastXPos-atualXPos)>=aux) break;
				}
				mymotors.SetSpeed(0,0); //stop motor
				//sleep(atoi(cmdValue.c_str()));
}

void backward(){
	
}

void right(){
	
}

void left(){
	
}


class Robot {
	public:
		Robot();

	private:
		/*PlayerClient robot;
		DioProxy systemDio;
		RangerProxy ranger;
		Position2dProxy base;
		Position2dProxy neck;
		BumperProxy bumper;
		PowerProxy power;

};
Robot::Robot(){
	PlayerClient robot(host,port);
	DioProxy systemDio(&robot,0);
	Proxy ranger(&robot,0);
	Position2dProxy base(&robot, 1);
	Position2dProxy neck(&robot, 0);
	BumperProxy bumper(&robot, 0);
	PowerProxy power(&robot, 0);
}
*/

double distanceTraveled(double x, double y){
	return sqrt(x*x+y*y);
}



int main(int argc, char** argv){
  int i,n;
  unsigned char value;
  string option;
  string host;
  int port;
  double lastTravel = 0;
  double lastAngularTravel = 0;
  double totalTravel = 0;
  double totalAngularTravel = 0;
  float stepLength = 0.05;

  //default arguments
  host = "localhost"; 
  port = 6665;

  //Arguments treatment
  for(i=1;i<argc;i++){      
	if(argv[i][0]=='-'&argv[i][1]=='h'){
	  host = argv[i+1];  
	  i=i+1;
	}
	else if(argv[i][0]=='-'&argv[i][1]=='p'){
	  port = atoi(argv[i+1]);  //convert to int
	  i=i+1;
	}
	else {
	  cout << endl << "./main [options]" << endl;
	  cout << "Where [options] can be:" << endl;
	  cout << "  -h <ip>        : host ip where Player is running.Default: localhost" << endl;
	  cout << "  -p <port>      : port where Player will listen. Default: 6665" << endl << endl;
	  return -1;
	}
  }


	PlayerClient    robot(host,port);
	DioProxy systemDio(&robot,0);
	RangerProxy myranger(&robot,0);
	Position2dProxy mymotors(&robot, 0);
	Position2dProxy neckServo(&robot, 1);
	BumperProxy mybumper(&robot, 0);
	PowerProxy mypower(&robot, 0);
	//BeepProxy mybeep(&robot, 0);
	SpeechProxy myspeech(&robot, 0);

  cout << "Client starts! " << endl;


  //keyboard configs //g
  struct controller cont;
  memset( &cont, 0, sizeof(cont) );
  pthread_t dummy;

  	float frontRangerStallSetpoint=FRONT_RANGER_STALL_SETPOINT; //[m]
	while( true ){
  		pthread_create(&dummy, NULL, &keyboard_handler, (void*)&cont);
		while(lastKey!='c'){
			if(cont.dirty){
				switch(lastKey){
					case 'C':
						//mybeep.playTone(NOTE_C6,200);
					break;
					case 'D':
						//mybeep.playTone(NOTE_D6,200);
					break;
					case 'E':
						//mybeep.playTone(NOTE_E6,200);
					break;
					case 'F':
						//mybeep.playTone(NOTE_F6,200);
					break;
					case 'G':
						//mybeep.playTone(NOTE_G6,200);
					break;
					case 'A':
						//mybeep.playTone(NOTE_A6,200);
					break;
					case 'B':
						//mybeep.playTone(NOTE_B6,200);
					break;
					default:
						robot.Read();
						if(myranger[1]>frontRangerStallSetpoint){
							if(!mybumper.IsAnyBumped()){
								float linear_default_vel = 0.04; //[m/s]
								if(cont.speed == 0 && cont.turnrate == 0) mymotors.SetSpeed(0,0);	//stop
								else if(cont.speed == 1 && cont.turnrate == 0) mymotors.SetSpeed(linear_default_vel,0); //forward
								else if(cont.speed == 0 && cont.turnrate == 1) mymotors.SetSpeed(0,linear_default_vel);//left
								else if(cont.speed == 0 && cont.turnrate == -1) mymotors.SetSpeed(0,-linear_default_vel); //right
								else if(cont.speed == -1 && cont.turnrate == 0) mymotors.SetSpeed(-linear_default_vel,0); //backward
							}
							else{
								mymotors.SetSpeed(0,0);	//stop
							}
						}
						else{
							mymotors.SetSpeed(0,0);	//stop
						}
					break;

				}
				cont.dirty = false; // we've handled the changes
			}
			else{
				if(cont.speed != 0 || cont.turnrate != 0){
					mymotors.SetSpeed(0,0);	//stop if robot is not stoped
					cont.speed = 0;
					cont.turnrate = 0;
				}
			}

			system("clear");
			puts("Reading from keyboard");
			puts("---------------------------");
			puts("Moving around:");
			puts("        i         c = command mode");
			puts("   j    k    l ");
			puts("        ,     ");
			puts("button release : stop");
			puts("---------------------------");
			cout << "lastKey:" << lastKey << endl;
			puts("---------------------------");

			/////////////////////////////////////////////Debugging method////////////////////////////////////////////////
			cout << "Debugging method:" << endl; //RightTick = XPos, RightSpeed = XSpeed

			cout << "Dio data:";
			value = systemDio.GetDigin(); //returns the decimal value stored in the systemDio (DioProxy).
			for(int j=4;j>=0;j--) cout << ((value >> j) & 1); //show value as binary
			cout << "  => Vib2, Vib1, Buzzer, Comm, Power.";
			cout << endl;

			cout << "XPOS:" << mymotors.GetXPos() << " YPOS:" << mymotors.GetYPos() << " YawPOS:" << mymotors.GetYaw() << endl;
			cout << "LSpeed:" << mymotors.GetYSpeed() << " Xspeed:" << mymotors.GetXSpeed() << endl;

			cout << "Power charge:" << mypower.GetCharge() << " Power percent:" << mypower.GetPercent() << " Power valid:" << mypower.IsValid() << endl;

			cout << "Bumpers:";
			if(mybumper.IsAnyBumped()){
				for(i=0;i<mybumper.GetCount();i++){  //print all lasers receiveds
					cout << (mybumper.IsBumped(i) ? "1 " : "0 ") << " ";
				}
				cout << endl;
				cout << "        NE N  NW SW S  SE" << endl;
			}
			else cout << "None bumpers had been bumped." << endl << endl;

			cout << "Rangers:";
			for(i=0;i<myranger.GetRangeCount();i++){  //print all lasers receiveds
				if((int(myranger[i]/10))==0) cout << "  " << myranger[i] << " ";  //unidade
				else if((int(myranger[i]/100))==0) cout << " " << myranger[i] << " ";  //dezena
				else cout << myranger[i] << " ";  //centena
			}
			cout << "Qnt:" << myranger.GetRangeCount() << endl;
			cout << "          1   2   3   4   5   6   7" << endl;
			//////////////////////////////////////////////////////////////////////////////////////////////////
			robot.Read(); //update proxies  // esta aqui no fim pq se botar antes ddos prints ele nao printa nada
		}

		tcsetattr(kfd, TCSANOW, &cooked); //exit raw mode
		pthread_cancel(dummy);
		while(lastKey!='q'){
			robot.Read();
			system("clear");
			puts("Reading from commands");
			puts("---------------------------");
			puts("pf pt pd pe = movimentation (foward backward right left)");
			puts("o = set output (VR VL BZ LC LP)");
			puts("q = teleop mode");
			puts("---------------------------");

			/////////////////////////////////////////////Debugging method////////////////////////////////////////////////
			cout << "Debugging method:" << endl; //RightTick = XPos, RightSpeed = XSpeed

			cout << "Dio data:";
			value = systemDio.GetDigin(); //returns the decimal value stored in the systemDio (DioProxy).
			for(int j=4;j>=0;j--) cout << ((value >> j) & 1); //show value as binary
			cout << "  => Vib2, Vib1, Buzzer, Comm, Power.";
			cout << endl;

			cout << "XPOS:" << mymotors.GetXPos() << " YPOS:" << mymotors.GetYPos() << " YawPOS:" << mymotors.GetYaw() << endl;
			cout << " XSpeed:" << mymotors.GetXSpeed() << "YSpeed:" << mymotors.GetYSpeed() << endl;
			cout << "Travel:" << totalTravel << endl;

			cout << "Power charge:" << mypower.GetCharge() << " Power percent:" << mypower.GetPercent() << " Power valid:" << mypower.IsValid() << endl;

			cout << "Bumpers:";
			if(mybumper.IsAnyBumped()){
				for(i=0;i<mybumper.GetCount();i++){  //print all lasers receiveds
					cout << (mybumper.IsBumped(i) ? '1' : '0') << " ";
				}
				cout << endl;
			}
			else cout << "None bumpers had been bumped." << endl;

			cout << "Rangers:";
			for(i=0;i<myranger.GetRangeCount();i++){  //print all lasers receiveds
				if((int(myranger[i]/10))==0) cout << "  " << myranger[i] << " ";  //unidade
				else if((int(myranger[i]/100))==0) cout << " " << myranger[i] << " ";  //dezena
				else cout << myranger[i] << " ";  //centena
			}
			cout << "Qnt:" << myranger.GetRangeCount() << endl;
			//////////////////////////////////////////////////////////////////////////////////////////////////


			cout << ">>";
			cin >> option;
			
			if(option.at(0)=='p'){
				string cmdValue;
				cin >> cmdValue;

				float linear_default_vel = 0.04; //[m/s]
				float aux;

				if(option.at(1)=='f'){
					//forward();
					aux = strtof(cmdValue.c_str(),NULL);
					mymotors.SetSpeed(linear_default_vel,0); //mymotors.SetSpeed(atoi(cmdValue.c_str()),0);
				}
				else if (option.at(1)=='d'){
					//right();
					aux = (3.1415*strtof(cmdValue.c_str(),NULL))/180;
					mymotors.SetSpeed(0,-linear_default_vel);
				}
				else if (option.at(1)=='e'){
					//left();
					aux = (3.1415*strtof(cmdValue.c_str(),NULL))/180;
					mymotors.SetSpeed(0,linear_default_vel);
				}
				else if (option.at(1)=='t'){
					//backward();
					aux = strtof(cmdValue.c_str(),NULL);
					mymotors.SetSpeed(-linear_default_vel,0);
				}
				

				double atualTravel = distanceTraveled(mymotors.GetXPos(),mymotors.GetYPos());
				double atualAngularTravel = fabs(mymotors.GetYaw());
				lastTravel = atualTravel;
				lastAngularTravel = atualAngularTravel;
				while(true){
					robot.Read();
					atualTravel = distanceTraveled(mymotors.GetXPos(),mymotors.GetYPos());
					atualAngularTravel = fabs(mymotors.GetYaw());
					if(option.at(1)=='f'       && (atualTravel-lastTravel)>=aux) break;
					else if (option.at(1)=='t' && (lastTravel-atualTravel)>=aux) break;		
					else if (option.at(1)=='d' && (atualAngularTravel-lastAngularTravel)>=aux) break;
					else if (option.at(1)=='e' && (lastAngularTravel-atualAngularTravel)>=aux) break;	

					if(mybumper.IsAnyBumped()) break;
					for(i=0;i<myranger.GetRangeCount();i++) if(myranger[i]<30 && (i<3||i==6)) cout << "VAI BATEEEEE!!!" << endl; 		
				}
				totalTravel += atualTravel; 
				totalAngularTravel += atualAngularTravel;
				mymotors.SetSpeed(0,0); //stop motor
			}
			else if(option.at(0)=='n'){
				string cmdValue;
				if(option.size()==1){ //evita comandos sem espaço ex: "n180" ao inves de "n 180"
					cin >> cmdValue;
					neckServo.SetSpeed(0,atoi(cmdValue.c_str()));		
				}
			}
			else if(option=="o"){
				string cmdValue;
				cin >> cmdValue;
				unsigned char value = 0;

				if(cmdValue.at(4)=='1') value |= 1 << 0; //-------x //set bit
				else value &= ~(1 << 0); //clear bit

				if(cmdValue.at(3)=='1') value |= 1 << 1; //------x- //set bit
				else value &= ~(1 << 1); //clear bit

				if(cmdValue.at(2)=='1') value |= 1 << 2; //-----x-- //set bit
				else value &= ~(1 << 2); //clear bit

				if(cmdValue.at(1)=='1') value |= 1 << 3; //----x--- //set bit
				else value &= ~(1 << 3); //clear bit

				if(cmdValue.at(0)=='1') value |= 1 << 4; //---x---- //set bit
				else value &= ~(1 << 4); //clear bit

				systemDio.SetOutput(8,value); //01110111 foward
				robot.Read(); //update proxies
			}
			else if(option=="q") lastKey = 'q';
		}
		tcsetattr(kfd, TCSANOW, &raw); //enter in raw mode
	}
	
	robot.Read(); //update proxies  
  return(0);
}


