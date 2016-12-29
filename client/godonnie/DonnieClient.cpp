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


#ifndef LANG 
#define LANG "pt-br"
#endif

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
	muted = false;

	try{
		robot = new PlayerClient(host,port);
	} catch (PlayerError e){
		#ifndef NDEBUG
			cerr << e << endl;
		#endif
		cerr << "Não foi possivel conectar no robô com IP " << host << " porta " << port << endl;
		cerr << "Possivelmente o Player não foi executado ou as variaveis DONNIE_IP e DONNIE_PORT estão erradas" << endl;
		exit(1);
	}
	
	try{		
		//head = new PlayerClient("localhost",6666);
		p2dProxy = new Position2dProxy(robot,0);
		p2d_headProxy = new Position2dProxy(robot,1);
		//actuator = new ActArrayProxy(robot,0);
		bpProxy = new BumperProxy(robot,0);
		bfinderProxy = new BlobfinderProxy(robot,0);
		sonarProxy = new RangerProxy(robot,0);
		headSonarProxy = new RangerProxy(robot,1);
		speechProxy = new SpeechProxy(robot,0);
	}catch (PlayerError e){
		#ifndef NDEBUG
			cerr << e << endl;
		#endif
		cerr << "Não foi possível conectar no robô " << endl;
		cerr << "Possivelmente o arquivo cfg está incorreto." << endl;
		exit(1);
	}
	
	robot->StartThread();
}


DonnieClient::~DonnieClient()
{

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



string DonnieClient::value_to_color(int color_value)
{
	string color;
	ostringstream oss;
	unsigned int file_value;

	ifstream file;
	string donnie_path = GetEnv("DONNIE_PATH");

	// read the color file according to the language
	if(LANG == "pt-br")
		donnie_path = donnie_path + "/resources/color_files/rgb-pt-br.txt";
	else if (LANG == "eng")
		donnie_path = donnie_path + "/resources/color_files/rgb-en.txt";
	else
		donnie_path = donnie_path + "/resources/color_files/rgb-pt-br.txt";

	file.open(donnie_path.c_str());

	if(file.is_open()) //if there is no problem opening the file
	{
		//unsigned int smaller= 0,smaller_buff=0xFFFFFFFF;
		//string color_buff;

		getline (file,color); // get first line
		while(getline(file,color,'\t')) //get the number of the color
		{

			stringstream str_buff;
			str_buff << std::hex << color; //get hex value and convert to string.. 
			str_buff >> file_value;		   //then put on an unsigned int 
			getline(file,color, '\n');
			
			if(file_value == color_value || file_value == 0xFFFFFFFF) 
			{
				return color;
			}
			//smaller =  color_value - file_value;
			//if(smaller_buff < smaller) 
			//{
			//	return color_buff;
			//}
			//smaller_buff = smaller;
			//color_buff = color;
		}
		file.close();
	}
	else{
		cout << "Erro: arquivo de cores " << donnie_path << " não encontrado" << endl;
		exit(1);
	}
	
	return color;
}

int DonnieClient::color_to_value(string input_color)
{
	ifstream file;
	string color;
	ostringstream oss;
	unsigned int file_value;

	string donnie_path = GetEnv("DONNIE_PATH");

	// read the color file according to the language
	if(LANG == "pt-br")
		donnie_path = donnie_path + "/resources/color_files/rgb-pt-br.txt";
	else if (LANG == "eng")
		donnie_path = donnie_path + "/resources/color_files/rgb-en.txt";
	else
		donnie_path = donnie_path + "/resources/color_files/rgb-pt-br.txt";

	file.open(donnie_path.c_str());
	if(file.is_open())
	{
		getline (file,color); // get first line
		while(getline(file,color,'\t')) //get the number of the color
		{

			stringstream str_buff;
			str_buff << std::hex << color; //convert 
			str_buff >> file_value;
			getline(file,color, '\n');


			if(input_color == color)
				return file_value;
		}
		file.close();
	}
	else{
		cout << "Erro: arquivo de cores " << donnie_path << " não encontrado" << endl;
		exit(1);
	}

	// return unknown color
	return 0xFFFFFFFF;
	
}


float DonnieClient::GetRange(int arg)
{
	try{
		robot->ReadIfWaiting();
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
/*
			case 6: //cabeca - head
				//return sonarProxy->GetRange(6)/STEP_LENGHT;
				return headSonarProxy->GetRange(0)/STEP_LENGHT;
*/				
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

int DonnieClient::GetPos(string p2d, int arg)
{
	if (p2d == "head")
		return (int)GetPos(p2d_headProxy, arg);
	else
		return (int)GetPos(p2dProxy, arg);
}

float DonnieClient::GetPos(Position2dProxy *p2d, int arg)
{
	robot->ReadIfWaiting();

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


int DonnieClient::moveForward(int arg)
{
	robot->ReadIfWaiting();
	double yaw = p2dProxy->GetYaw();    //Angulo do robo
	double posxi = p2dProxy->GetXPos();   //Posicao inicial X do robo
	double posyi = p2dProxy->GetYPos();   //Posicao inicial Y do robo

	int Npassos = arg;
	int passos = 0;

	bool obstacle = false;
	bool stop = true;

	// movement stoped due to nearby obstacle
	robot->ReadIfWaiting();
	if(sonarProxy->GetRange(1) > 0.15)
	{
	  stop = false;
	  p2dProxy->SetSpeed(0.05,0);
	}

	robot->ReadIfWaiting();
	while(!stop)
	{
      // movement finished as expected
	  if(passos >= Npassos )
	  {
	    p2dProxy->SetSpeed(0,0);
	    break;
	  }
	
      // movement stoped due to collision
	  robot->ReadIfWaiting();
	  if(this->FrontBumper() != 0 or sonarProxy->GetRange(1) < FRONT_RANGER or sonarProxy->GetRange(2) < SIDE_RANGER or sonarProxy->GetRange(0) < SIDE_RANGER)
	  {
	    stop = true;
	    p2dProxy->SetSpeed(0,0);
	    break;
	  }
	    
      // movement stoped due to nearby obstacle
	  robot->ReadIfWaiting();
	  if(sonarProxy->GetRange(1) < 0.20 and !obstacle)
	  {
	    obstacle = true;
	    p2dProxy->SetSpeed(0,0);
		break;
	  }

	  robot->ReadIfWaiting();
	  if(hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi) > (STEP_LENGHT*(passos+1)))
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

	// say command
	std::ostringstream sayStr;
		sayStr << "Andei " << int(passos) << " passos para frente.";
	if (bumped() || stop)
		sayStr << "Houve colisão.";
	if (obstacle)
		sayStr << " Encontrei obstáculo.";
	speak(sayStr.str());

	#ifndef NDEBUG
	cout << "parou: " << stop << ", obstaculo: " << obstacle << endl;
	#endif
	
	// number of steps actually taken
	return passos;
}

int DonnieClient::moveBackward(int arg)
{
	robot->ReadIfWaiting();
	double yaw = p2dProxy->GetYaw();    //Angulo do robo
	double posxi = p2dProxy->GetXPos();   //Posicao inicial X do robo
	double posyi = p2dProxy->GetYPos();   //Posicao inicial Y do robo

	int Npassos = arg;
	int passos = 0;

	bool obstacle = false;
	bool stop = true;

	// movement stoped due to nearby obstacle
	if(sonarProxy->GetRange(4) > 0.2)
	{
	  stop = false;
	  p2dProxy->SetSpeed(-0.05,0);
	}

	while(!stop)
	{
	  // movement finished as expected
	  if(passos >= Npassos )
	  {
	    p2dProxy->SetSpeed(0,0);
	    break;
	  }

      // movement stoped due to collision
	  robot->ReadIfWaiting();
	  if(this->BackBumper() != 0 or sonarProxy->GetRange(4) < BACK_RANGER or sonarProxy->GetRange(3) < SIDE_RANGER or sonarProxy->GetRange(5) < SIDE_RANGER)
	  {
	    stop = true;
	    p2dProxy->SetSpeed(0,0);
	    break;
	  }
	    
      // movement stoped due to nearby obstacle
	  robot->ReadIfWaiting();
	  if(sonarProxy->GetRange(4) < 0.2 and !obstacle) // For donnie, do not work in stage.
	  {
	    obstacle = true;
	    p2dProxy->SetSpeed(0,0);
	    break;
	  }

	  robot->ReadIfWaiting();
	  if(hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi) > (STEP_LENGHT*(passos+1)))
	  {
	  		passos++;
	  }

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
	// say command
	std::ostringstream sayStr;
	sayStr << "Andei " << passos << " passos para trás.";
	if (bumped() || stop)
		sayStr << "Houve colisão.";
	if (obstacle)
		sayStr << ". Encontrei obstáculo.";
	speak(sayStr.str());
		
	#ifndef NDEBUG
	cout << "parou: " << stop << ", obstaculo: " << obstacle << endl;
	#endif

	// number of steps actually taken
	return passos;	
}

int DonnieClient::GotoTTS(float pa){

	// call recursive Goto and return the actual move
	//float actual_move = Goto(pa);
	Goto(pa);
	
	string direction;
	if (pa < 0){
		direction = "direita";
		pa = -pa;
	}else
		direction = "esquerda";
	// say command
	// TODO: o comando pode ser interrompido por uma colizao.
	// assim, está errado assumir que a distancia pedida será a distancia percorrida
	std::ostringstream sayStr;
	sayStr << "Girei " << int(pa) << " graus para " << direction << ".";
	if (bumped())
		sayStr << " Houve colisão.";
	speak(sayStr.str());

	return 0;	
}

int DonnieClient::Goto(float pa){
	//faz com que o goto funcione girando para o lado correto
	DEBUG_MSG("ROTACAO:" << pa);
	if(pa>0 && pa>170){
		DEBUG_MSG("PARCIAL");
		if(Goto(170)) return 1; //se qualquer batida ou erro acontecer retorna imediatamente
		if(Goto(pa-170)) return 1; //se qualquer batida ou erro acontecer retorna imediatamente
		return 0;
	}
	if(pa<0 && pa<-170){	
		DEBUG_MSG("PARCIAL");
		if(Goto(-170)) return 1; //se qualquer batida ou erro acontecer retorna imediatamente
		if(Goto(pa+170)) return 1; //se qualquer batida ou erro acontecer retorna imediatamente
		return 0;
	}

	float errorOffset = DTOR(0.5); //min value that is considered at correct angle
	float paTarget = DTOR(pa)+p2dProxy->GetYaw(); //target = desejado + atual
	if(paTarget>0 && paTarget>M_PI) paTarget=paTarget-2*M_PI; //ajusta o valor do target evitando que o robo fique infinitamente tentando girar
	if(paTarget<0 && paTarget<-M_PI) paTarget=paTarget+2*M_PI;
	do{
		robot->ReadIfWaiting();
		p2dProxy->GoTo(p2dProxy->GetXPos(),p2dProxy->GetYPos(),paTarget); //soma o angulo desejado com o angulo inicial do robo
		//DEBUG_MSG("           "<< "TH POS:" << p2dProxy->GetYaw());
		//DEBUG_MSG("           "<< "TARGET:" << (paTarget)<<endl);
	}while (p2dProxy->GetYaw()<=(paTarget)-errorOffset || 
			p2dProxy->GetYaw()>=(paTarget)+errorOffset); 
	p2dProxy->SetSpeed(0,0);
	return 0;
}



int DonnieClient::headGoto(float pa){
	float errorOffsetYaw = DTOR(0.7); //min value that is considered at correct angle
	float paTarget = DTOR(pa);
	do{
		robot->ReadIfWaiting();
		p2d_headProxy->GoTo(p2dProxy->GetXPos(),p2dProxy->GetYPos(), paTarget+p2dProxy->GetYaw()); //soma a posicao desejada da cabeca em relacao a base
		//DEBUG_MSG("           "<< "TH POS:" << p2d_headProxy->GetYaw());
		//DEBUG_MSG("           "<< "TARGET:" << (paTarget+p2dProxy->GetYaw())-errorOffsetYaw<<endl);
	}while (p2d_headProxy->GetYaw()<=(paTarget+p2dProxy->GetYaw())-errorOffsetYaw || 
			p2d_headProxy->GetYaw()>=(paTarget+p2dProxy->GetYaw())+errorOffsetYaw);  //0.5 is the parameter to validate the speed more fast
	p2d_headProxy->SetSpeed(0,0);
	return 0;
}

void DonnieClient::Scan(void){
	float head_yawi = -90; //in degree. +90 due the servo default pos is 90 degre
	//GOTO -90 to 90 in 30 by 30 steps
	float sonar_readings[7];
	int blobs_found[7];
	int yaw_cnt=0;
			
	std::ostringstream scanText;
	string color_str;
	speak("Espiando");
	do{
		// move head
		headGoto(head_yawi);
		robot->ReadIfWaiting();
		// read sonar
		headSonarProxy->GetRange(0)/100; ///STEP_LENGHT;  // read head sonar 
		sonar_readings[yaw_cnt] = headSonarProxy->GetRange(0)/STEP_LENGHT;  // read head sonar 
		blobs_found[yaw_cnt] = bfinderProxy->GetCount(); // get the number of blobs found
		
		// get the color of the blobs
		color_str = "";
        for(int i = 0; i < blobs_found[yaw_cnt]; i++)
        {
			// color is encodedd in 0x00RRGGBB format	
			color_str+=value_to_color(bfinderProxy->GetBlob(i).color);
			// if it is the last
			if (i+1 != blobs_found[yaw_cnt])
				color_str += ",";
		}		
		
		// build string
		if (head_yawi == 0)
			scanText << "a frente: ";
		else if (head_yawi < 0)
			scanText << "a " << -head_yawi << " graus a direita: ";
		else 
			scanText << "a " << head_yawi << " graus a esquerda: ";

		// OBS: '(int)*sonar_readings' truncate the distance. perhaps 'round' would be better
		if (blobs_found[yaw_cnt] == 0){
			scanText << "0 objetos a " << (int)sonar_readings[yaw_cnt] << " passos";
		}else if (blobs_found[yaw_cnt] == 1){
			scanText << "1 objeto de cor " << color_str << " a " << (int)sonar_readings[yaw_cnt] << " passos";
		}else{
			scanText << blobs_found[yaw_cnt] << " objetos de cores " << color_str << " a " << (int)sonar_readings[yaw_cnt] << " passos";
		}
		
		speak(scanText.str());
		// TODO gambiarra. deveria ter um método WaitUntilPlayed p aguardar o fim do audio
		sleep(2);

		scanText.str("");
		scanText.clear();	
		head_yawi = head_yawi + 30; // more + 30 degree 
		yaw_cnt++;
	}while (head_yawi < (90+30));

	// go back to the initial position
	headGoto(0);
	robot->ReadIfWaiting(); 

	#ifndef NDEBUG
		int graus = 0;
		cout << "SCAN: "<< endl;
		for(int i=0; i<7; i++){
		   cout << blobs_found[i] << " objetos a " << int(sonar_readings[i]) << " passos no grau " << graus << endl; 
		   graus+=30;
		}              
		cout << endl;
	#endif
}

int DonnieClient::Color(int color_code){
	float head_yawi = -90; //in degree. +90 due the servo default pos is 90 degre
	//GOTO -90 to 90 in 30 by 30 steps
	int blobs_found = 0;
	int total_blobs_found = 0;

	std::ostringstream scanText;
	std:string color_str;
	color_str = value_to_color(color_code);
	speak("Procurando cor " + color_str);
	do{
		// move head
		headGoto(head_yawi);
		robot->ReadIfWaiting();
		
		// get the color of the blobs
        for(int i = 0; i < bfinderProxy->GetCount(); i++)
        {
        	cout<< value_to_color(bfinderProxy->GetBlob(i).color)<<endl;
        	cout<<std::hex <<bfinderProxy->GetBlob(i).color<<endl;
			if (color_str == value_to_color(bfinderProxy->GetBlob(i).color))
				blobs_found++;
		}
		
		// build string
		if (head_yawi == 0)
			scanText << "a frente: ";
		else if (head_yawi < 0)
			scanText << "a " << -head_yawi << " graus a direita: ";
		else 
			scanText << "a " << head_yawi << " graus a esquerda: ";

		// OBS: '(int)*sonar_readings' truncate the distance. perhaps 'round' would be better
		if (blobs_found == 0){
			scanText << "0 objetos";
		}else if (blobs_found == 1){
			scanText << "1 objeto";
		}else{
			scanText << blobs_found << " objetos"; 
		}			

		speak(scanText.str());
		// TODO gambiarra. deveria ter um método WaitUntilPlayed p aguardar o fim do audio
		sleep(2);

		scanText.str("");
		scanText.clear();	
		head_yawi = head_yawi + 30; // more + 30 degree 
		total_blobs_found += blobs_found;
		blobs_found=0;
	}while (head_yawi < (90+30));
	
	// go back to the initial position
	headGoto(0);
	robot->ReadIfWaiting();

	// generate output
	if (total_blobs_found == 0){
		scanText << "nenhum objeto encontrado com a cor " << color_str;
	}else 	if (total_blobs_found == 1){
		scanText << "1 objeto encontrado com a cor " << color_str;
	}else	{
		scanText << total_blobs_found << " objetos encontrados com a cor " << color_str;
	}
	speak(scanText.str());
	
	return total_blobs_found;
}

int DonnieClient::bumped(){
	if (FrontBumper()==0 && BackBumper()==0 && !p2dProxy->GetStall())
		return 0;
	else
		return 1;
}


void DonnieClient::speak(string text)
{
	if (muted)
		cout << text << endl;
	else{
		speechProxy->Say(text.c_str());
		// TODO gambiarra
		sleep(2);
	}	
}

void DonnieClient::muteTTS(bool m)
{
	muted = m;
}
