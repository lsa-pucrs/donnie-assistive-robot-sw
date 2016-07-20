#include "DonnieClient.h"
#include <libplayerc++/playerc++.h>

using namespace std;
using namespace PlayerCc;


DonnieClient::DonnieClient()
{
  robot = new PlayerClient("localhost",6665);
  //head = new PlayerClient("localhost",6666);

  p2dProxy = new Position2dProxy(robot,0);

  //p2d_headProxy = new Position2dProxy(robot,1);

  //actuator = new ActArrayProxy(robot,0);

  bpProxy = new BumperProxy(robot,0);

  //BfinderProxy = new BlobfinderProxy(head,0);

  sonarProxy = new RangerProxy(robot,0);
  //SHProxy = new RangerProxy(head,3);

  //speech = new SpeechProxy(robot,0);

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
	robot->Read();
	switch(arg)
	{
		case 0:
			return sonarProxy->GetRange(0);

		case 1:
			return sonarProxy->GetRange(1);

		case 2:
			return sonarProxy->GetRange(2);

		case 3:
			return sonarProxy->GetRange(3);

		case 4:
			return sonarProxy->GetRange(4);

		case 5:
			return sonarProxy->GetRange(5);
	}
}

float DonnieClient::GetPos(int arg)
{
	robot->Read();

	switch(arg)
	{
		case 0:
			return p2dProxy->GetXPos();

		case 1:
			return p2dProxy->GetYPos();

		case 2:
			return radTOdeg(p2dProxy->GetYaw());
	}

}


void DonnieClient::ParaFrente(float arg)
{
	vector<float> path;

	robot->Read();
	double yaw = p2dProxy->GetYaw();    //Angulo do robo
	double posxi = p2dProxy->GetXPos();   //Posicao inicial X do robo
	double posyi = p2dProxy->GetYPos();   //Posicao inicial Y do robo


	for(int i = 1; i <= (int)arg; i++)
	{

	  path.push_back(i);
	  cout << path[i-1] << endl;
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
	if(sonarProxy->GetRange(1) > 1.6) // For donnie, do not work in stage.
	{
	  stop = false;
	  //p2d_headProxy->SetSpeed(1,0);
	  p2dProxy->SetSpeed(1,0);
	}

	robot->Read();
	while(!stop)
	{

		//cout << passos << " > " << Npassos << endl;
	  if(passos > Npassos )
	  {
	  	//p2d_headProxy->SetSpeed(0,0);
	    p2dProxy->SetSpeed(0,0);
	    break;
	  }

	  robot->ReadIfWaiting();
	  if(this->FrontBumper() != 0 or sonarProxy->GetRange(1) < 0.4 or sonarProxy->GetRange(2) < 0.65 or sonarProxy->GetRange(0) < 0.65) // For donnie, do not work in stage.
	  {
	  	//p2d_headProxy->SetSpeed(0,0);
	    p2dProxy->SetSpeed(0,0);
	    stop = true;
	    andou = sqrt(pow((p2dProxy->GetXPos() - posxi),2) + pow((p2dProxy->GetYPos() - posyi),2));
	    erro = andou - (int)andou;
	    break;
	  }
	    
	  robot->ReadIfWaiting();
	  if(sonarProxy->GetRange(1) < 1.5 and !obstacle) // For donnie, do not work in stage.
	  {
	    Npassos = passos;
	    obstacle = true;
	  }

	  robot->ReadIfWaiting();
	  if(hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi) > path[passos] - 0.15)
	  {
	  		passos++;
	  }
	    
	  //if(yaw >= 0)
	  //{
	  //  if(yaw < M_PI/2)
	  //  {
	  //    robot->ReadIfWaiting();
	  //    if(p2dProxy->GetXPos() >= path[passos].posx and p2dProxy->GetYPos() >= path[passos].posy)
	  //    {
	  //      passos++;
	  //    }
	  //  }
	  //  else
	  //  {
	  //    robot->ReadIfWaiting();
	  //    if(p2dProxy->GetXPos() <= path[passos].posx and p2dProxy->GetYPos() >= path[passos].posy)
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
	  //    if(p2dProxy->GetXPos() >= path[passos].posx and p2dProxy->GetYPos() <= path[passos].posy)
	  //    {
	  //      passos++;
	  //    }
	  //  }
	  //  else
	  //  {
	  //    robot->ReadIfWaiting();
	  //    if(p2dProxy->GetXPos() <= path[passos].posx and p2dProxy->GetXPos() <= path[passos].posy)
	  //    {
	  //      passos++;
	  //    }
	  //  }  
	  //}
	}

	if(stop = true and erro < 0.8 and erro > 0.2)
	{
		cout << "!"  << erro << endl;
		this->ParaTras(erro);
	}

	path.clear();
	//robot->Read();
	//cout << p2dProxy->GetXPos() << ", " << p2dProxy->GetYPos() << endl;
}

void DonnieClient::ParaTras(float arg)
{
	vector<float> path;

	robot->Read();
	double yaw = p2dProxy->GetYaw();    //Angulo do robo
	double posxi = p2dProxy->GetXPos();   //Posicao inicial X do robo
	double posyi = p2dProxy->GetYPos();   //Posicao inicial Y do robo

	PathNodes Point;

	for(int i = 1; i <= (int)arg; i++)
	{

	  path.push_back(i);
	  cout << path[i-1] << endl;
	}

	float fltPart = arg - (int)arg;

	if(fltPart > 0)
	{
	  path.push_back(fltPart);
	  cout << path[0];
	}

	float Npassos = arg;
	int passos = 0;

	bool obstacle = false;
	bool stop = true;
	float erro;
	float andou;

	if(sonarProxy->GetRange(3) > 1.6) // For donnie, do not work in stage.
	{
	  stop = false;
	  //p2d_headProxy->SetSpeed(-1,0);
	  p2dProxy->SetSpeed(-1,0);
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
	  if(this->BackBumper() != 0 or sonarProxy->GetRange(4) < 0.4 or sonarProxy->GetRange(3) < 0.65 or sonarProxy->GetRange(5) < 0.65) // For donnie, do not work in stage.
	  {
	  	//p2d_headProxy->SetSpeed(0,0);
	    p2dProxy->SetSpeed(0,0);
	    stop = true;
	    andou = sqrt(pow((p2dProxy->GetXPos() - posxi),2) + pow((p2dProxy->GetYPos() - posyi),2));
	    erro = andou - (int)andou;
	    break;
	  }
	    
	  robot->ReadIfWaiting();
	  if(sonarProxy->GetRange(4) < 1.5 and !obstacle) // For donnie, do not work in stage.
	  {
	    Npassos = passos;
	    obstacle = true;
	  }

	  robot->ReadIfWaiting();
	  if(hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi) > path[passos] - 0.15)
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

	if(stop = true and erro < 0.8 and erro > 0.2)
	{
		cout << "!"  << endl<< endl;
		this->ParaFrente(erro);
	}

	path.clear();
	//robot->Read();
	//cout << p2dProxy->GetXPos() << ", " << p2dProxy->GetYPos() << endl;
}

void DonnieClient::ParaDireita(float arg)
{
	robot->Read();

	double yawf = p2dProxy->GetYaw(); //Angulo "bruto" atual do robo (radianos negativos e positivos)
	double yawi;  //Angulo atual do robo (apenas radianos positivos)
	double yawd;  //Angulo de destino do robo
	yawi = radTOrad(yawf);

	cout << yawi << endl;

	/////////////////Calculo para determinar angulo que o robo deve chegar////////////////////////////////
	if(yawi - degTOrad(arg) > M_PI)
	  yawd = (yawi - degTOrad(arg)) - 2*M_PI;
	else if(yawi - degTOrad(arg) < -1*M_PI)
	  yawd = (yawi - degTOrad(arg)) + 2*M_PI;
	else
	  yawd = yawi - degTOrad(arg);

	cout << yawd << endl;

	//p2d_headProxy->SetSpeed(0,-0.5);
	p2dProxy->SetSpeed(0,-0.5);
	while(true)
	{
	  robot->ReadIfWaiting();
	  if(p2dProxy->GetYaw() >= 0 and yawd >= 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2dProxy->GetYaw() < M_PI/2 and yawd < M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() > yawd + 0.000001)
	      {
	        break;
	      }
	    }
	    else if(p2dProxy->GetYaw() >= M_PI/2 and yawd >= M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() > yawd + 0.000001)
	      {
	        break;
	      }
	    }
	    else
	    {
	      break;
	    }
	  }
	  else if(p2dProxy->GetYaw() < 0 and yawd < 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2dProxy->GetYaw() < -M_PI/2 and yawd < -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() > yawd + 0.000001)
	      {
	        break;
	      }
	    }
	    else if(p2dProxy->GetYaw() >= -M_PI/2 and yawd >= -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() > yawd + 0.000001)
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
	  if(p2dProxy->GetYaw() >= 0 and yawd >= 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2dProxy->GetYaw() < M_PI/2 and yawd < M_PI/2)
	    {
	      break;
	    }
	    else if(p2dProxy->GetYaw() >= M_PI/2 and yawd >= M_PI/2)
	    {
	      break;
	    }

	  }
	  else if(p2dProxy->GetYaw() < 0 and yawd < 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2dProxy->GetYaw() < -M_PI/2 and yawd < -M_PI/2)
	    {
	        break;
	    }
	    else if(p2dProxy->GetYaw() >= -M_PI/2 and yawd >= -M_PI/2)
	    {	      
	        break;
	    }
	  }
	}

	robot->ReadIfWaiting();

	while(true)
	{
	  robot->ReadIfWaiting();
	  if(p2dProxy->GetYaw() >= 0 and yawd >= 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2dProxy->GetYaw() < M_PI/2 and yawd < M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() <= yawd + 0.0500000)
	      {
	        break;
	      }
	    }
	    else if(p2dProxy->GetYaw() >= M_PI/2 and yawd >= M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() <= yawd + 0.0500000)
	      {
	        break;
	      }
	    }
	    else
	    {
	      break;
	    }
	  }
	  else if(p2dProxy->GetYaw() < 0 and yawd < 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2dProxy->GetYaw() < -M_PI/2 and yawd < -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() <= yawd + 0.0500000)
	      {
	        break;
	      }
	    }
	    else if(p2dProxy->GetYaw() >= -M_PI/2 and yawd >= -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() <= yawd + 0.0500000)
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
	p2dProxy->SetSpeed(0,0);
	robot->Read();
	cout << p2dProxy->GetYaw() << endl;
}

void DonnieClient::ParaEsquerda(float arg)
{
	robot->Read();

	double yawf = p2dProxy->GetYaw(); //Angulo "bruto" atual do robo (radianos negativos e positivos)
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
	p2dProxy->SetSpeed(0,0.5);

	while(true)
	{
	  robot->ReadIfWaiting();
	  if(p2dProxy->GetYaw() >= 0 and yawd >= 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2dProxy->GetYaw() < M_PI/2 and yawd < M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() < yawd - 0.000001)
	      {
	        break;
	      }
	    }
	    else if(p2dProxy->GetYaw() >= M_PI/2 and yawd >= M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() < yawd - 0.000001)
	      {
	        break;
	      }
	    }
	    else
	    {
	      break;
	    }
	  }
	  else if(p2dProxy->GetYaw() < 0 and yawd < 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2dProxy->GetYaw() < -M_PI/2 and yawd < -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() < yawd - 0.000001)
	      {
	        break;
	      }
	    }
	    else if(p2dProxy->GetYaw() >= -M_PI/2 and yawd >= -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() < yawd - 0.000001)
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
	  if(p2dProxy->GetYaw() >= 0 and yawd >= 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2dProxy->GetYaw() < M_PI/2 and yawd < M_PI/2)
	    {
	      break;
	    }
	    else if(p2dProxy->GetYaw() >= M_PI/2 and yawd >= M_PI/2)
	    {
	      break;
	    }

	  }
	  else if(p2dProxy->GetYaw() < 0 and yawd < 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2dProxy->GetYaw() < -M_PI/2 and yawd < -M_PI/2)
	    {
	        break;
	    }
	    else if(p2dProxy->GetYaw() >= -M_PI/2 and yawd >= -M_PI/2)
	    {	      
	        break;
	    }
	  }
	}

	robot->ReadIfWaiting();
	while(true)
	{
	  robot->ReadIfWaiting();
	  if(p2dProxy->GetYaw() >= 0 and yawd >= 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2dProxy->GetYaw() < M_PI/2 and yawd < M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() >= yawd - 0.0500000)
	      {
	        break;
	      }
	    }
	    else if(p2dProxy->GetYaw() >= M_PI/2 and yawd >= M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() >= yawd - 0.0500000)
	      {
	        break;
	      }
	    }
	    else
	    {
	      break;
	    }
	  }
	  else if(p2dProxy->GetYaw() < 0 and yawd < 0)
	  {
	    robot->ReadIfWaiting();
	    if(p2dProxy->GetYaw() < -M_PI/2 and yawd < -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() >= yawd - 0.0500000)
	      {
	        break;
	      }
	    }
	    else if(p2dProxy->GetYaw() >= -M_PI/2 and yawd >= -M_PI/2)
	    {
	      robot->ReadIfWaiting();
	      if(p2dProxy->GetYaw() >= yawd - 0.0500000)
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
	p2dProxy->SetSpeed(0,0);
}



