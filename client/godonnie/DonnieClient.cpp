#include "DonnieClient.h"
#include "Historic.h"
#include <libplayerc++/playerc++.h>

using namespace std;
using namespace PlayerCc;

//Trocar por const
// TODO: amory. criar include donnie_defs.h com todas as definicoes de tamanho do donnie
// TODO: amory. duplicaco c alerts.cc
const double STEP_LENGHT = 0.05;
const double  SIDE_RANGER = 0.05;
const double  FRONT_RANGER = 0.06;
const double  BACK_RANGER = 0.05;


//Singleton. init pointer withou allocations
DonnieClient *DonnieClient::singleton = NULL;

DonnieClient *DonnieClient::getInstance()
{
   if (!singleton)   // Only allow one instance of class to be generated.
      singleton = new DonnieClient();
   return singleton;
}

void DonnieClient::ResetInstance()
{
      delete singleton; 
      singleton = NULL; 
}

DonnieClient::DonnieClient()
{
	string host = GetEnv("DONNIE_IP");
	int port = atoi(GetEnv("DONNIE_PORT").c_str());
	if(host.size()==0) host = "localhost";
	if(port==0) port = 6665;

	try{
		robot = new PlayerClient(host,port);
	} catch (PlayerError e){
		#ifndef NDEBUG
			cerr << e << endl;
		#endif
		cerr << "Nao foi possivel conectar no robo com IP " << host << " porta " << port << endl;
		cerr << "Possivelmente o Player nao foi executado ou as variaveis DONNIE_IP e DONNIE_PORT estao erradas" << endl;
		exit(1);
	}
	
	try{		
		//head = new PlayerClient("localhost",6666);
		p2dProxy = new Position2dProxy(robot,0);
		p2d_headProxy = new Position2dProxy(robot,1);
		//actuator = new ActArrayProxy(robot,0);
		bpProxy = new BumperProxy(robot,0);
		//BfinderProxy = new BlobfinderProxy(head,0);
		sonarProxy = new RangerProxy(robot,0);
		headSonarProxy = new RangerProxy(robot,1);
		speechProxy = new SpeechProxy(robot,0);
	}catch (PlayerError e){
		#ifndef NDEBUG
			cerr << e << endl;
		#endif
		cerr << "Nao foi possivel conectar no robo " << endl;
		cerr << "Possivelmente o arquivo cfg esta incorreto." << endl;
		exit(1);
	}
	
	robot->StartThread();
}


DonnieClient::~DonnieClient()
{
	/*
	//head = new PlayerClient("localhost",6666);
	delete p2dProxy;
	delete p2d_headProxy;
	delete bpProxy;
	//BfinderProxy = new BlobfinderProxy(head,0);
	delete sonarProxy;
	//SHProxy = new RangerProxy(head,3);
	delete speech;	
	delete robot;
	* */
}
	
int DonnieClient::FrontBumper()
{
  robot->ReadIfWaiting();

  if(bpProxy->IsBumped(0) and bpProxy->IsBumped(1))
    return 1;
  else if(bpProxy->IsBumped(0))
    return 2;
  else if(bpProxy->IsBumped(1))
    return 3;
  else
    return 0;

}

int DonnieClient::BackBumper()
{
  robot->ReadIfWaiting();

  if(bpProxy->IsBumped(2) and bpProxy->IsBumped(3))
    return 1;
  else if(bpProxy->IsBumped(2))
    return 2;
  else if(bpProxy->IsBumped(3))
    return 3;
  else
    return 0;

}

float DonnieClient::GetRange(int arg)
{
	try{
		robot->Read();
		switch(arg)
		{   // The order is NE N  NW SW S  SE HEAD
			case 0: //frente - N
				return sonarProxy->GetRange(1)/STEP_LENGHT; // /STEP_LENGHT to convert from m to steps

			case 1: //tras - S
				return sonarProxy->GetRange(4)/STEP_LENGHT;

			case 2: //frente-esquerda - NW 
				return sonarProxy->GetRange(2)/STEP_LENGHT;

			case 3: //frente-direita - NE
				return sonarProxy->GetRange(0)/STEP_LENGHT;

			case 4: //tras-esquerda - SW
				return sonarProxy->GetRange(3)/STEP_LENGHT;

			case 5: //tras-direita - SE
				return sonarProxy->GetRange(5)/STEP_LENGHT;

			case 6: //cabeca - head
				return sonarProxy->GetRange(6)/STEP_LENGHT;
				
			default: // invalid
				ostringstream buf;
				buf << "Range id "<< arg << "invalid" << endl;
				throw PlayerError("DonnieClient::GetRange()", buf.str());
		}
	}
	catch (PlayerError e)
    {
      std::cerr << e << std::endl;
      return -1.0;
    }
}

float DonnieClient::GetPos(string p2d, int arg)
{
	if (p2d == "head")
		return GetPos(p2d_headProxy, arg);
	else
		return GetPos(p2dProxy, arg);
}

float DonnieClient::GetPos(Position2dProxy *p2d, int arg)
{
	robot->Read();

	switch(arg)
	{
		case 0:
			return p2d->GetXPos()/STEP_LENGHT;
		case 1:
			return p2d->GetYPos()/STEP_LENGHT;
		case 2:
			return radTOdeg(p2d->GetYaw());
		default:
			ostringstream buf;
			buf << "Position id "<< arg << "invalid" << endl;
			throw PlayerError("DonnieClient::GetPos()", buf.str());
			return -1.0;
	}

}


int DonnieClient::moveForward(float arg)
{
	vector<float> path;

	robot->Read();
	double yaw = p2dProxy->GetYaw();    //Angulo do robo
	double posxi = p2dProxy->GetXPos();   //Posicao inicial X do robo
	double posyi = p2dProxy->GetYPos();   //Posicao inicial Y do robo


	for(int i = 1; i <= (int)arg; i++)
	{

	  path.push_back(i*STEP_LENGHT);
	  #ifndef NDEBUG // only print when it was compiled in debug mode. 'cmake -DCMAKE_BUILD_TYPE=Debug ..'
	  cout << path[i-1] << endl;
	  #endif
	}

	float fltPart = arg - (int)arg;

	if(fltPart > 0)
	{
	  path.push_back(fltPart);
	}

	float Npassos = arg;
	int passos = 0;

	bool obstacle = false;
	bool stop = true;
	float erro;
	float andou;

	robot->Read();
	if(sonarProxy->GetRange(1) > 0.15)
	{
	  stop = false;
	  p2d_headProxy->SetSpeed(0,1);
	  p2dProxy->SetSpeed(0.05,0);
	}

	robot->Read();
	while(!stop)
	{

		//cout << passos << " > " << Npassos << endl;
	  if(passos > Npassos )
	  {
	  	p2d_headProxy->SetSpeed(0,0);
	    p2dProxy->SetSpeed(0,0);
	    break;
	  }

	  robot->ReadIfWaiting();
	  if(this->FrontBumper() != 0 or sonarProxy->GetRange(1) < FRONT_RANGER or sonarProxy->GetRange(2) < SIDE_RANGER or sonarProxy->GetRange(0) < SIDE_RANGER)
	  {
	  	p2d_headProxy->SetSpeed(0,0);
	    p2dProxy->SetSpeed(0,0);
	    stop = true;
	    andou = hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi);
	    erro = andou - (int)andou;
	    break;
	  }
	    
	  robot->ReadIfWaiting();
	  if(sonarProxy->GetRange(1) < 0.20 and !obstacle)
	  {
	    Npassos = passos + 1;
	    obstacle = true;
	  }

	  robot->ReadIfWaiting();
	  if(hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi) > path[passos])
	  {
	  		passos++;
	  }
	}
/* se bateu, para mas nao volta
	if(stop = true and erro < 0.8 and erro > 0.2)
	{
		#ifndef NDEBUG
		cout << "!"  << erro << endl;
		#endif
		this->moveBackward(erro);
	}
*/
	path.clear();

	#ifndef NDEBUG
	cout << "Andou: " << passos << ", parou: " << stop << ", erro: " << erro << ", obstaculo: " << obstacle << endl;
	#endif
	
	// number of steps actually taken
	return passos;
}

int DonnieClient::moveBackward(float arg)
{
	vector<float> path;

	robot->Read();
	double yaw = p2dProxy->GetYaw();    //Angulo do robo
	double posxi = p2dProxy->GetXPos();   //Posicao inicial X do robo
	double posyi = p2dProxy->GetYPos();   //Posicao inicial Y do robo

	PathNodes Point;

	for(int i = 1; i <= (int)arg; i++)
	{

	  path.push_back(i*STEP_LENGHT);
	  #ifndef NDEBUG
	  cout << path[i-1] << endl;
	  #endif
	}

	float fltPart = arg - (int)arg;

	if(fltPart > 0)
	{
	  path.push_back(fltPart);
	  #ifndef NDEBUG
	  cout << path[0];
	  #endif
	}

	float Npassos = arg;
	int passos = 0;

	bool obstacle = false;
	bool stop = true;
	float erro;
	float andou;

	if(sonarProxy->GetRange(4) > 0.2)
	{
	  stop = false;
	  //p2d_headProxy->SetSpeed(-1,0);
	  p2dProxy->SetSpeed(-0.05,0);
	}

	while(!stop)
	{
	  if(passos > Npassos )
	  {
	  	//p2d_headProxy->SetSpeed(0,0);
	    p2dProxy->SetSpeed(0,0);
	    break;
	  }

	  robot->ReadIfWaiting();
	  if(this->BackBumper() != 0 or sonarProxy->GetRange(4) < BACK_RANGER or sonarProxy->GetRange(3) < SIDE_RANGER or sonarProxy->GetRange(5) < SIDE_RANGER)
	  {
	  	//p2d_headProxy->SetSpeed(0,0);
	    p2dProxy->SetSpeed(0,0);
	    stop = true;
	    andou = hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi);
	    erro = andou - (int)andou;
	    break;
	  }
	    
	  robot->ReadIfWaiting();
	  if(sonarProxy->GetRange(4) < 0.2 and !obstacle) // For donnie, do not work in stage.
	  {
	    Npassos = passos;
	    obstacle = true;
	  }

	  robot->ReadIfWaiting();
	  if(hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi) > path[passos])
	  {
	  		passos++;
	  }
	    
	  //if(yaw > 0)
	  //{
	  //  if(yaw < M_PI/2)
	  //  {
	  //    robot->ReadIfWaiting();
	  //    if(p2dProxy->GetXPos() <= path[passos].posx and p2dProxy->GetYPos() <= path[passos].posy)
	  //    {
	  //      passos++;
	  //    }
	  //  }
	  //  else
	  //  {
	  //    robot->ReadIfWaiting();
	  //    if(p2dProxy->GetXPos() >= path[passos].posx and p2dProxy->GetYPos() <= path[passos].posy)
	  //    {
	  //      passos++;
	  //    }
	  //  } 
	  //}
	  //else
	  //{
	  //  if(yaw > -M_PI/2)
	  //  {
	  //    robot->ReadIfWaiting();
	  //    if(p2dProxy->GetXPos() <= path[passos].posx and p2dProxy->GetYPos() >= path[passos].posy)
	  //    {
	  //      passos++;
	  //    }
	  //  }
	  //  else
	  //  {
	  //    robot->ReadIfWaiting();
	  //    if(p2dProxy->GetXPos() >= path[passos].posx and p2dProxy->GetYPos() >= path[passos].posy)
	  //    {
	  //      passos++;
	  //    }
	  //  }  
	  //}
	}

/* se bateu, para mas nao volta
	if(stop = true and erro < 0.8 and erro > 0.2)
	{	
		#ifndef NDEBUG
		cout << "!"  << endl<< endl;
		#endif
		this->moveForward(erro);
	}
*/
	path.clear();
	
	#ifndef NDEBUG
	cout << "Andou: " << passos << ", parou: " << stop << ", erro: " << erro << ", obstaculo: " << obstacle << endl;
	#endif

	// number of steps actually taken
	return passos;	
}

void DonnieClient::turnRight(string p2d, float arg)
{
	if (p2d == "head")
		turnRight(p2d_headProxy, arg);
	else
		turnRight(p2dProxy, arg);
}

void DonnieClient::turnRight(Position2dProxy *p2d, float arg)
{
	robot->Read();

	double yawf = p2d->GetYaw(); //Angulo "bruto" atual do robo (radianos negativos e positivos)
	double yawi;  //Angulo atual do robo (apenas radianos positivos)
	double yawd;  //Angulo de destino do robo
	yawi = radTOrad(yawf);

	#ifndef NDEBUG
	cout << yawi << endl;
	#endif

	/////////////////Calculo para determinar angulo que o robo deve chegar////////////////////////////////
	if(yawi - degTOrad(arg) > M_PI)
	  yawd = (yawi - degTOrad(arg)) - 2*M_PI;
	else if(yawi - degTOrad(arg) < -1*M_PI)
	  yawd = (yawi - degTOrad(arg)) + 2*M_PI;
	else
	  yawd = yawi - degTOrad(arg);

	#ifndef NDEBUG
	cout << yawd << endl;
	#endif

	//p2d_headProxy->SetSpeed(0,-0.5);
	p2d->SetSpeed(0,-0.5);
	while(true)
	{
	  robot->ReadIfWaiting();
	  if(p2d->GetYaw() >= 0 and yawd >= 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2d->GetYaw() < M_PI/2 and yawd < M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() > yawd + 0.000001)
	      {
	        break;
	      }
	    }
	    else if(p2d->GetYaw() >= M_PI/2 and yawd >= M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() > yawd + 0.000001)
	      {
	        break;
	      }
	    }
	    else
	    {
	      break;
	    }
	  }
	  else if(p2d->GetYaw() < 0 and yawd < 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2d->GetYaw() < -M_PI/2 and yawd < -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() > yawd + 0.000001)
	      {
	        break;
	      }
	    }
	    else if(p2d->GetYaw() >= -M_PI/2 and yawd >= -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() > yawd + 0.000001)
	      {
	        break;
	      }
	    }
	    else
	    {
	      break;
	    }
	  }
	  else
	  {
	    break;
	  }
	}

	robot->ReadIfWaiting();

	while(true)
	{
	  robot->ReadIfWaiting();
	  if(p2d->GetYaw() >= 0 and yawd >= 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2d->GetYaw() < M_PI/2 and yawd < M_PI/2)
	    {
	      break;
	    }
	    else if(p2d->GetYaw() >= M_PI/2 and yawd >= M_PI/2)
	    {
	      break;
	    }

	  }
	  else if(p2d->GetYaw() < 0 and yawd < 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2d->GetYaw() < -M_PI/2 and yawd < -M_PI/2)
	    {
	        break;
	    }
	    else if(p2d->GetYaw() >= -M_PI/2 and yawd >= -M_PI/2)
	    {	      
	        break;
	    }
	  }
	}

	robot->ReadIfWaiting();

	while(true)
	{
	  robot->ReadIfWaiting();
	  if(p2d->GetYaw() >= 0 and yawd >= 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2d->GetYaw() < M_PI/2 and yawd < M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() <= yawd + 0.0500000)
	      {
	        break;
	      }
	    }
	    else if(p2d->GetYaw() >= M_PI/2 and yawd >= M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() <= yawd + 0.0500000)
	      {
	        break;
	      }
	    }
	    else
	    {
	      break;
	    }
	  }
	  else if(p2d->GetYaw() < 0 and yawd < 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2d->GetYaw() < -M_PI/2 and yawd < -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() <= yawd + 0.0500000)
	      {
	        break;
	      }
	    }
	    else if(p2d->GetYaw() >= -M_PI/2 and yawd >= -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() <= yawd + 0.0500000)
	      {
	        break;
	      }
	    }
	    else
	    {
	      break;
	    }
	  }
	  else
	  {
	    break;
	  }
	}

	//p2d_headProxy->SetSpeed(0,0);
	p2d->SetSpeed(0,0);
	robot->Read();
	#ifndef NDEBUG
	cout << p2d->GetYaw() << endl;
	#endif
}

void DonnieClient::turnLeft(string p2d, float arg)
{
	if (p2d == "head")
		turnLeft(p2d_headProxy, arg);
	else
		turnLeft(p2dProxy, arg);
}

void DonnieClient::turnLeft(Position2dProxy *p2d,float arg)
{
	robot->Read();

	double yawf = p2d->GetYaw(); //Angulo "bruto" atual do robo (radianos negativos e positivos)
	double yawi;  //Angulo atual do robo (apenas radianos positivos)
	double yawd;  //Angulo de destino do robo
	yawi = radTOrad(yawf);

	/////////////////Calculo para determinar angulo que o robo deve chegar////////////////////////////////
	if(yawi + degTOrad(arg) > 3*M_PI)
	  yawd = (yawi + degTOrad(arg)) - 4*M_PI;
	else if(yawi + degTOrad(arg) > M_PI)
	  yawd = (yawi + degTOrad(arg)) - 2*M_PI;
	else
	  yawd = yawi + degTOrad(arg);

	//p2d_headProxy->SetSpeed(0,0.5);
	p2d->SetSpeed(0,0.5);

	while(true)
	{
	  robot->ReadIfWaiting();
	  if(p2d->GetYaw() >= 0 and yawd >= 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2d->GetYaw() < M_PI/2 and yawd < M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() < yawd - 0.000001)
	      {
	        break;
	      }
	    }
	    else if(p2d->GetYaw() >= M_PI/2 and yawd >= M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() < yawd - 0.000001)
	      {
	        break;
	      }
	    }
	    else
	    {
	      break;
	    }
	  }
	  else if(p2d->GetYaw() < 0 and yawd < 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2d->GetYaw() < -M_PI/2 and yawd < -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() < yawd - 0.000001)
	      {
	        break;
	      }
	    }
	    else if(p2d->GetYaw() >= -M_PI/2 and yawd >= -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() < yawd - 0.000001)
	      {
	        break;
	      }
	    }
	    else
	    {
	      break;
	    }
	  }
	  else
	  {
	    break;
	  }
	}

	while(true)
	{
	  robot->ReadIfWaiting();
	  if(p2d->GetYaw() >= 0 and yawd >= 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2d->GetYaw() < M_PI/2 and yawd < M_PI/2)
	    {
	      break;
	    }
	    else if(p2d->GetYaw() >= M_PI/2 and yawd >= M_PI/2)
	    {
	      break;
	    }

	  }
	  else if(p2d->GetYaw() < 0 and yawd < 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2d->GetYaw() < -M_PI/2 and yawd < -M_PI/2)
	    {
	        break;
	    }
	    else if(p2d->GetYaw() >= -M_PI/2 and yawd >= -M_PI/2)
	    {	      
	        break;
	    }
	  }
	}

	robot->ReadIfWaiting();
	while(true)
	{
	  robot->ReadIfWaiting();
	  if(p2d->GetYaw() >= 0 and yawd >= 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2d->GetYaw() < M_PI/2 and yawd < M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() >= yawd - 0.0500000)
	      {
	        break;
	      }
	    }
	    else if(p2d->GetYaw() >= M_PI/2 and yawd >= M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() >= yawd - 0.0500000)
	      {
	        break;
	      }
	    }
	    else
	    {
	      break;
	    }
	  }
	  else if(p2d->GetYaw() < 0 and yawd < 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2d->GetYaw() < -M_PI/2 and yawd < -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() >= yawd - 0.0500000)
	      {
	        break;
	      }
	    }
	    else if(p2d->GetYaw() >= -M_PI/2 and yawd >= -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2d->GetYaw() >= yawd - 0.0500000)
	      {
	        break;
	      }
	    }
	    else
	    {
	      break;
	    }
	  }
	  else
	  {
	    break;
	  }
	}
	//p2d_headProxy->SetSpeed(0,0);
	p2d->SetSpeed(0,0);
}
int DonnieClient::Goto(float px, float py, float pa){
	float errorOffset=0.8;
	if(pa>0){
		do{
			robot->ReadIfWaiting();
			p2d_headProxy->GoTo(px,py,DTOR(pa));
		}while (p2d_headProxy->GetYaw()>=DTOR(pa-errorOffset)); //0.5 is the parameter to validate the speed more fast
		p2d_headProxy->SetSpeed(0,0);
	}
	else{
		do{
			robot->ReadIfWaiting();
			p2d_headProxy->GoTo(px,py,DTOR(pa));
		}while (p2d_headProxy->GetYaw()<=DTOR(pa+errorOffset)); //0.5 is the parameter to validate the speed more fast
		p2d_headProxy->SetSpeed(0,0);
	}
	return 1;
}

void DonnieClient::Scan(float *sonar_readings){
	float errorOffset=0.8;
	do{ //GOTO -90
		robot->ReadIfWaiting();
		p2d_headProxy->GoTo(p2d_headProxy->GetXPos(),p2d_headProxy->GetYPos(), DTOR(-90));
	}while (p2d_headProxy->GetYaw()>=-1*DTOR(90-errorOffset));  //0.5 is the parameter to validate the speed more fast
	p2d_headProxy->SetSpeed(0,0);

	float head_yawi = RTOD(p2d_headProxy->GetYaw()); //in degree. +90 due the servo default pos is 90 degre
	do{//GOTO -90 to 90 in 30 by 30 steps
		do{
			robot->ReadIfWaiting();
			p2d_headProxy->GoTo(p2d_headProxy->GetXPos(),p2d_headProxy->GetYPos(), DTOR(head_yawi));
		}while (p2d_headProxy->GetYaw()<=DTOR(head_yawi-errorOffset));  //0.5 is the parameter to validate the speed more fast
		p2d_headProxy->SetSpeed(0,0);

		robot->ReadIfWaiting();
		headSonarProxy->GetRange(0)/100; ///STEP_LENGHT;  // read head sonar 
		*sonar_readings = headSonarProxy->GetRange(0)/STEP_LENGHT;  // read head sonar 
		cout << "           "<< "TH POS:" << RTOD(p2d_headProxy->GetYaw()) << endl;
		cout << "           "<< "TH SPEED:" << p2d_headProxy->GetYawSpeed() << endl;
		cout << "           "<< "TARGET:" << DTOR(head_yawi) << endl;
		if(head_yawi<1&&head_yawi>-1) cout << "           "<< "FOWARD SONAR:" << sonarProxy->GetRange(1)/STEP_LENGHT << endl; //debug para comparaçao
		cout << "           "<< "HEAD SONAR:" << *sonar_readings << endl << endl;
		sonar_readings++;

		head_yawi = head_yawi + 30; // more + 30 degree 
	}while (head_yawi < (90+30));

	do{ //GOTO 0
		robot->ReadIfWaiting();
		p2d_headProxy->GoTo(p2d_headProxy->GetXPos(),p2d_headProxy->GetYPos(), DTOR(0));
	}while (p2d_headProxy->GetYaw()>=DTOR(errorOffset));  //0.5 is the parameter to validate the speed more fast
	p2d_headProxy->SetSpeed(0,0);
}

int DonnieClient::bumped(){
	if (FrontBumper()==0 && BackBumper()==0 && !p2dProxy->GetStall())
		return 0;
	else
		return 1;
}


void DonnieClient::speak(string text)
{
	cout << text << endl;
	speechProxy->Say(text.c_str());
}