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

	// Set up language environment
	generator gen;
	gen.add_messages_path(string(GetEnv("DONNIE_PATH")) + "/resources/loc");
	gen.add_messages_domain("DonnieClient");
	locale loc = gen(string(GetEnv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);
	cout.imbue(loc);
	cerr.imbue(loc);

	try{
		robot = new PlayerClient(host,port);
	} catch (PlayerError e){
		#ifndef NDEBUG
			cerr << e << endl;
		#endif
		cerr << translate("Não foi possivel conectar no robô com IP ") << host << translate(" porta ")
			 << port << endl;
		cerr << translate("Possivelmente o Player não foi executado ou as variaveis DONNIE_IP e DONNIE_PORT estão erradas")
			 << endl;
		exit(1);
	}

	try{
		p2dProxy = new Position2dProxy(robot,0);
		p2d_headProxy = new Position2dProxy(robot,1);
		bpProxy = new BumperProxy(robot,0);
		bfinderProxy = new BlobfinderProxy(robot,0);
		sonarProxy = new RangerProxy(robot,0);
		headSonarProxy = new RangerProxy(robot,1);
		speechProxy = new SpeechProxy(robot,0);
	}catch (PlayerError e){
		#ifndef NDEBUG
			cerr << e << endl;
		#endif
		cerr << translate("Não foi possível conectar no robô ") << endl;
		cerr << translate("Possivelmente o arquivo cfg está incorreto.") << endl;
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
	if(strcmp(getenv("DONNIE_LANG"),"pt_BR") == 0)
		donnie_path = donnie_path + "/resources/color_files/rgb-pt-br.txt";
	else if (strcmp(getenv("DONNIE_LANG"),"en_US") == 0)
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
		cout << translate("Erro: arquivo de cores ") << donnie_path << translate(" não encontrado") << endl;
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

	// Set up language environment
	generator gen;
	gen.add_messages_path(string(GetEnv("DONNIE_PATH")) + "/resources/loc");
	gen.add_messages_domain("DonnieClient");
	locale loc = gen(string(GetEnv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);
	cout.imbue(loc);
	cerr.imbue(loc);

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
		cout << translate("Erro: arquivo de cores ") << donnie_path << translate(" não encontrado") << endl;
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
	std::ostringstream sayStr;

	// Set up language environment
	generator gen;
	gen.add_messages_path(string(GetEnv("DONNIE_PATH")) + "/resources/loc");
	gen.add_messages_domain("DonnieClient");
	locale loc = gen(string(GetEnv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);
	sayStr.imbue(loc);

	switch(arg)
	{
		case 0:
			return p2d->GetXPos()/STEP_LENGHT;
		case 1:
			return p2d->GetYPos()/STEP_LENGHT;
		case 2:
			return radTOdeg(p2d->GetYaw());
		case 3:
			sayStr << string(translate("Estou no x ")) << (int)(p2d->GetXPos()/STEP_LENGHT) << string(translate(", no y ")) << (int)(p2d->GetYPos()/STEP_LENGHT) << string(translate(" e virado para ")) << (int)(radTOdeg(p2d->GetYaw())) << string(translate(" graus."));
			speak(sayStr.str());
			return 0;
		default:
			ostringstream buf;
			buf << "Position id "<< arg << "invalid" << endl;
			throw PlayerError("DonnieClient::GetPos()", buf.str());
			return -1.0;
	}

}

int DonnieClient::meterToSteps(double m){
	int steps;
	steps=int(m/STEP_LENGHT);
	return steps;
}

int DonnieClient::moveForward(int arg)
{
	robot->Read();
	//double yawi = p2dProxy->GetYaw();    //Angulo do robo
	double posxi = p2dProxy->GetXPos();   //Posicao inicial X do robo
	double posyi = p2dProxy->GetYPos();   //Posicao inicial Y do robo

	int passos = 0;
	int Npassos = arg;

	bool obstacle = false;
	bool collision = false;

	double targetHypot = STEP_LENGHT*Npassos;

	// Set up language environment
	generator gen;
	gen.add_messages_path(string(GetEnv("DONNIE_PATH")) + "/resources/loc");
	gen.add_messages_domain("DonnieClient");
	locale loc = gen(string(GetEnv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);

	//validate step number
	if(Npassos != 0){
		//initial collision prevent - check if not are nearby obstacle to start movement
		robot->Read();
		if(sonarProxy->GetRange(1) > 2*FRONT_RANGER)//Range South
			p2dProxy->SetSpeed(0.05,0);
		else obstacle = true;

		//hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi) <= targetHypot
		while(passos < Npassos && collision==false && obstacle==false)
		{
			//#ifndef NDEBUG
			//cout << "targetHypot:" << targetHypot << endl;
			//cout << "Hypot:" << hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi) << endl;
			//cout << "===============PF-PRECISION-TEST=================" << endl;
			//#endif

			//collision detect
			if(this->FrontBumper() != 0){
				p2dProxy->SetSpeed(0,0);
				collision = true;
			}

			robot->ReadIfWaiting();
			//collision prevent
			if(sonarProxy->GetRange(2) < SIDE_RANGER || sonarProxy->GetRange(1) < FRONT_RANGER || sonarProxy->GetRange(0) < SIDE_RANGER){
				p2dProxy->SetSpeed(0,0);
				obstacle = true;
			}

			//steps incrementation
			if(hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi) >= (STEP_LENGHT*(passos+1))){
				passos++;
			}
			robot->ReadIfWaiting();
		}
	}
	p2dProxy->SetSpeed(0,0);

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
	sayStr.imbue(loc);
	string aux1 = string(translate("somente "));
	sayStr << string(translate("Andei ")) << ((collision||obstacle)?aux1:"")  << int(passos) << string(translate(" passos para frente."));
	if (collision)
		sayStr << string(translate("Houve colisão;")); //colisao frontal
	if (obstacle)
		sayStr << string(translate("Encontrei obstáculo."));
	speak(sayStr.str());
	//cout << sayStr.str() << endl;

	/*#ifndef NDEBUG
	cout << "obstaculo: " << obstacle << endl;
	#endif*/

	// number of steps actually taken
	return passos;
}

int DonnieClient::moveBackward(int arg)
{
	robot->Read();
	//double yawi = p2dProxy->GetYaw();    //Angulo do robo
	double posxi = p2dProxy->GetXPos();   //Posicao inicial X do robo
	double posyi = p2dProxy->GetYPos();   //Posicao inicial Y do robo

	int passos = 0;
	int Npassos = arg;

	bool obstacle = false;
	bool collision = false;

	double targetHypot = STEP_LENGHT*Npassos;

	// Set up language environment
	generator gen;
	gen.add_messages_path(string(GetEnv("DONNIE_PATH")) + "/resources/loc");
	gen.add_messages_domain("DonnieClient");
	locale loc = gen(string(GetEnv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);

	//validate step number
	if(Npassos != 0){
		//initial collision prevent - check if not are nearby obstacle to start movement
		robot->Read();
		if(sonarProxy->GetRange(4) > 2*FRONT_RANGER)//Range South
			p2dProxy->SetSpeed(-0.05,0);
		else obstacle = true;

		// hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi) <= targetHypot
		while(passos < Npassos && collision==false && obstacle==false)
		{
			//#ifndef NDEBUG
			//cout << "targetHypot:" << targetHypot << endl;
			//cout << "Hypot:" << hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi) << endl;
			//cout << "===============PF-PRECISION-TEST=================" << endl;
			//#endif

			//collision detect
			if(this->FrontBumper() != 0){
				p2dProxy->SetSpeed(0,0);
				collision = true;
			}

			robot->ReadIfWaiting();
			//collision prevent
			if(sonarProxy->GetRange(4) < SIDE_RANGER || sonarProxy->GetRange(3) < FRONT_RANGER || sonarProxy->GetRange(5) < SIDE_RANGER){
				p2dProxy->SetSpeed(0,0);
				obstacle = true;
			}

			//steps incrementation
			if(hypotf(p2dProxy->GetXPos() - posxi, p2dProxy->GetYPos() - posyi) >= (STEP_LENGHT*(passos+1))){
				passos++;
			}
			robot->ReadIfWaiting();
		}
	}
	p2dProxy->SetSpeed(0,0);

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
	sayStr.imbue(loc);
	string aux1 = string(translate("somente "));
	sayStr << string(translate("Andei ")) << ((collision||obstacle)?aux1:"")  << int(passos) << string(translate(" passos para trás."));
	if (collision)
		sayStr << string(translate("Houve colisão;")); //colisao frontal
	if (obstacle)
		sayStr << string(translate("Encontrei obstáculo."));
	speak(sayStr.str());
	//cout << sayStr.str() << endl;

	/*#ifndef NDEBUG
	cout << "obstaculo: " << obstacle << endl;
	#endif*/

	// number of steps actually taken
	return passos;
}

int DonnieClient::GotoTTS(float pa){

	// call recursive Goto and return the actual move
	//float actual_move = Goto(pa);
	Goto(pa);
	//make sure that robot are stopped while speech
	p2dProxy->SetSpeed(0,0);

	// Set up language environment
	generator gen;
	gen.add_messages_path(string(GetEnv("DONNIE_PATH")) + "/resources/loc");
	gen.add_messages_domain("DonnieClient");
	locale loc = gen(string(GetEnv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);

	string direction;
	if (pa < 0){
		direction = string(translate("direita"));
		pa = -pa;
	}else
		direction = string(translate("esquerda"));
	// say command
	// TODO: o comando pode ser interrompido por uma colizao.
	// assim, está errado assumir que a distancia pedida será a distancia percorrida
	std::ostringstream sayStr;
	sayStr.imbue(loc);
	sayStr << string(translate("Girei ")) << int(pa) << string(translate(" graus para ")) << direction << ".";
	if (bumped())
		sayStr << string(translate(" Houve colisão."));
	speak(sayStr.str());

	return 0;
}

int DonnieClient::Goto(float pa){
	//faz com que o goto funcione girando para o lado correto

	generator gen;
	gen.add_messages_path(string(GetEnv("DONNIE_PATH")) + "/resources/loc");
	gen.add_messages_domain("DonnieClient");
	locale loc = gen(string(GetEnv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);

	DEBUG_MSG(string(translate("ROTACAO:")) << pa);
	if(pa>0 && pa>170){
		DEBUG_MSG(string(translate("PARCIAL")));
		if(Goto(170)) return 1; //se qualquer batida ou erro acontecer retorna imediatamente
		if(Goto(pa-170)) return 1; //se qualquer batida ou erro acontecer retorna imediatamente
		return 0;
	}
	if(pa<0 && pa<-170){
		DEBUG_MSG(string(translate("PARCIAL")));
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
	int blobs_counter_buffer;
	std::ostringstream scanText;
	string color_str;

	// Set up language environment
	generator gen;
	gen.add_messages_path(string(GetEnv("DONNIE_PATH")) + "/resources/loc");
	gen.add_messages_domain("DonnieClient");
	locale loc = gen(string(GetEnv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);

	bool blob_flag = false;
	int camera_width = bfinderProxy->GetWidth() - 1;
	int nro_blobs = 0;
	int nro_blobs_buffer = 0;

	int yaw_buffer = 0;
	playerc_blobfinder_blob_t blob_buffer;
	blob_buffer.color =0;

	playerc_blobfinder_blob_t total_blobs_found[20];
	playerc_blobfinder_blob_t _total_blobs_found[20];
	int total_yaws[20];
	int _total_yaws[20];
	int total_counter=0;
	int graus = 0;

	//playerc_blobfinder_blob_t teste;


	speak(string(translate("Espiando")));
	do{
		// move headd
		headGoto(head_yawi);
		robot->ReadIfWaiting();
		sleep(1);
		// read sonar
		headSonarProxy->GetRange(0)/100; ///STEP_LENGHT;  // read head sonar

		sonar_readings[yaw_cnt] = headSonarProxy->GetRange(0)/STEP_LENGHT;  // read head sonar
		blobs_found[yaw_cnt] = bfinderProxy->GetCount(); // get the number of blobs found
		blobs_counter_buffer = blobs_found[yaw_cnt];




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		nro_blobs_buffer = bfinderProxy->GetCount();
		if(nro_blobs_buffer == 0 ) // caso nao tenha blobs, ainda tem que analisar pra ver se tem algum incompleto.
		{
			if (blob_flag ==true)
			{
				total_blobs_found[total_counter] = blob_buffer;
				total_yaws[total_counter] = yaw_buffer;
				total_counter++;
				//nro_blobs++;
				blob_flag = false;
			}
		}
		else
		{
			for(int b = 0; b<nro_blobs_buffer;b++)
			{

				usleep(100);
				int buffer_color =bfinderProxy->GetBlob(b).color;
				if(bfinderProxy->GetBlob(b).right != (camera_width) && bfinderProxy->GetBlob(b).left==0) /// inacabado na ESQUERDA.
				{
					if (blob_flag ==true)
					{
						if(blob_buffer.color !=buffer_color) // blobs with same colors! Para evitar que pegue a mesma blob 2x. Pode ocorrer outros erros.
						{
							//add the buffered blob.
							total_blobs_found[total_counter] = blob_buffer;
							total_yaws[total_counter] = yaw_buffer;
							total_counter++;
							yaw_buffer  = head_yawi;
						}

					}
					else
					{
						yaw_buffer = head_yawi;
						blob_flag = true;
					}
				}
				else if(bfinderProxy->GetBlob(b).right == (camera_width) && bfinderProxy->GetBlob(b).left !=0) //inacabado na DIREITA.
				{
					if (blob_flag ==true)
					{
						if(blob_buffer.color == buffer_color) // blobs with same colors!
						{
							total_blobs_found[total_counter] = blob_buffer;
							//mod aqui
							total_yaws[total_counter] = (head_yawi+yaw_buffer)/2;
							total_counter++;
							blob_flag = false;
						}
						else  // blobs with different colors!
						{
							//add the previous blob
							total_blobs_found[total_counter] = blob_buffer;
							total_yaws[total_counter] = yaw_buffer;
							total_counter++;


							total_blobs_found[total_counter] =  bfinderProxy->GetBlob(b);
							//mod aqui
							total_yaws[total_counter] = head_yawi;//graus;
							total_counter++;
							blob_flag = false;
						}
					}
					else
					{
						if(blob_buffer.color != buffer_color) // blobs with same colors!Para evitar que pegue a mesma blob 2x. Pode ocorrer outros erros.
						{
							total_blobs_found[total_counter] =  bfinderProxy->GetBlob(b);
							//mod aqui
							total_yaws[total_counter] = head_yawi;//graus;
							total_counter++;
						}

					}
				}
				else if(bfinderProxy->GetBlob(b).right == (camera_width) && bfinderProxy->GetBlob(b).left==0) // inacabado nas DUAS pontas.
				{
					if (blob_flag ==true)
					{
						if(blob_buffer.color == bfinderProxy->GetBlob(b).color)
						{
							blob_buffer = bfinderProxy->GetBlob(b);
						}
						else
						{
							//add the previous blob
							total_blobs_found[total_counter] = blob_buffer;
							total_yaws[total_counter] = yaw_buffer;
							total_counter++;
							//mod aqui
							yaw_buffer = head_yawi;//graus;

						}

					}
					else
					{
						yaw_buffer = head_yawi;//graus;
						blob_flag = true;
					}
				}
				else // ok, tudo certo :}
				{
					if (blob_flag ==true)
					{

						if(blob_buffer.color == buffer_color)
						{
							total_blobs_found[total_counter] =  bfinderProxy->GetBlob(b);
							//mod aqui
							total_yaws[total_counter] = head_yawi;//graus;
							total_counter++;
							blob_flag = false;
						}
						else
						{
							//add the buffered blob.
							total_blobs_found[total_counter] = blob_buffer;
							total_yaws[total_counter] = yaw_buffer;
							total_counter++;
							blob_flag = false;
							//then buffers the current blob.
							total_blobs_found[total_counter] =  blob_buffer;
							//mod aqui
							total_yaws[total_counter] = head_yawi;//graus;
							total_counter++;
						}

					}
					else
					{
						total_blobs_found[total_counter] =  bfinderProxy->GetBlob(b);
						//mod aqui
						total_yaws[total_counter] = head_yawi;// graus;
						total_counter++;
					}
				}
			blob_buffer = bfinderProxy->GetBlob(b);
			}
		}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		graus +=30;
		// TODO gambiarra. deveria ter um método WaitUntilPlayed p aguardar o fim do audio
		sleep(2);
		head_yawi = head_yawi + 30; // more + 30 degree
		yaw_cnt++;
	}while (head_yawi < (90+30));
	if(blob_flag ==true)
	{
		total_blobs_found[total_counter] = blob_buffer;
		total_yaws[total_counter] = yaw_buffer;
		total_counter++;
		blob_flag = false;
	}
	//gambiarra
	nro_blobs= 0;

	for(int i=0; i<total_counter; i++)
	{
		string buff = value_to_color(total_blobs_found[i].color);
		if((buff.compare(string(translate("desconhecido"))) != 0) && (total_yaws[i] <360))
		{
			_total_blobs_found[nro_blobs] = total_blobs_found[i];
			_total_yaws[nro_blobs] = total_yaws[i];
			nro_blobs++;
		}
	}
	// go back to the initial position
	headGoto(0);
	robot->ReadIfWaiting();

	scanText.imbue(loc);
	for(int i=0; i<nro_blobs; i++)
	{
		// build string
		if (_total_yaws[i] == 0)
			scanText << string(translate("a frente: "));
		else if (_total_yaws[i] < 0)
			scanText << string(translate("a ")) << -_total_yaws[i] << string(translate(" graus a direita: "));
		else
			scanText << string(translate("a ")) << _total_yaws[i] << string(translate(" graus a esquerda: "));

		scanText << string(translate(" um objeto de cor ")) << value_to_color(_total_blobs_found[i].color) << string(translate(" a ")) << int(sonar_readings[i]) << string(translate(" passos"));

		speak(scanText.str());

		// TODO gambiarra. deveria ter um método WaitUntilPlayed p aguardar o fim do audio
		//sleep(2);
		scanText.str("");
		scanText.clear();

	}
}

int DonnieClient::Color(int color_code){


	float head_yawi = -90; //in degree. +90 due the servo default pos is 90 degre
	std:string color_str;
	color_str = value_to_color(color_code);

	generator gen;
	gen.add_messages_path(string(GetEnv("DONNIE_PATH")) + "/resources/loc");
	gen.add_messages_domain("DonnieClient");
	locale loc = gen(string(GetEnv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);

	speak(string(translate("Procurando cor ")) + color_str);






	//GOTO -90 to 90 in 30 by 30 steps
	float sonar_readings[7];
	int yaw_cnt=0;
	int blobs_counter_buffer;
	std::ostringstream scanText;
	//string color_str;




	bool blob_flag = false;
	int camera_width = bfinderProxy->GetWidth() - 1;
	int nro_blobs = 0;
	int nro_blobs_buffer = 0;

	int yaw_buffer = 0;
	playerc_blobfinder_blob_t blob_buffer;
	blob_buffer.color =0;

	playerc_blobfinder_blob_t total_blobs_found[20];
	playerc_blobfinder_blob_t _total_blobs_found[20];
	int total_yaws[20];
	int _total_yaws[20];
	int total_counter=0;
	int graus = 0;


	speak(string(translate("Espiando")));
	do{
		// move headd
		headGoto(head_yawi);
		robot->ReadIfWaiting();
		sleep(1);

		// read sonar
		headSonarProxy->GetRange(0)/100; ///STEP_LENGHT;  // read head sonar
		sonar_readings[yaw_cnt] = headSonarProxy->GetRange(0)/STEP_LENGHT;  // read head sonar

		nro_blobs_buffer = bfinderProxy->GetCount();
		if(nro_blobs_buffer == 0 ) // caso nao tenha blobs, ainda tem que analisar pra ver se tem algum incompleto.
		{
			if (blob_flag ==true)
			{
					total_blobs_found[total_counter] = blob_buffer;
					total_yaws[total_counter] = yaw_buffer;
					total_counter++;
					blob_flag = false;
			}
		}
		else
		{
			for(int b = 0; b<nro_blobs_buffer;b++)
			{
				usleep(100);
				int buffer_color = bfinderProxy->GetBlob(b).color;

				if(bfinderProxy->GetBlob(b).right != (camera_width) && bfinderProxy->GetBlob(b).left==0) /// inacabado na ESQUERDA.
				{
					if (blob_flag ==true)
					{
						if(blob_buffer.color !=buffer_color) // blobs with same colors! Para evitar que pegue a mesma blob 2x. Pode ocorrer outros erros.
						{
							total_blobs_found[total_counter] = blob_buffer;
							total_yaws[total_counter] = yaw_buffer;
							total_counter++;
							yaw_buffer = graus;
						}

					}
					else
					{
						yaw_buffer = graus;
						blob_flag = true;
					}
				}
				else if(bfinderProxy->GetBlob(b).right == (camera_width) && bfinderProxy->GetBlob(b).left !=0) //inacabado na DIREITA.
				{
					if (blob_flag ==true)
					{
						if(blob_buffer.color == buffer_color) // blobs with same colors!
						{
							total_blobs_found[total_counter] = blob_buffer;
							total_yaws[total_counter] = (graus+yaw_buffer)/2;
							total_counter++;
							blob_flag = false;
						}
						else  // blobs with different colors!
						{
							//add the previous blob
							total_blobs_found[total_counter] = blob_buffer;
							total_yaws[total_counter] = yaw_buffer;
							total_counter++;


							total_blobs_found[total_counter] =  bfinderProxy->GetBlob(b);
							total_yaws[total_counter] = graus;
							total_counter++;
							blob_flag = false;
						}
					}
					else
					{
						if(blob_buffer.color != buffer_color) // blobs with same colors!Para evitar que pegue a mesma blob 2x. Pode ocorrer outros erros.
						{
							total_blobs_found[total_counter] =  bfinderProxy->GetBlob(b);
							total_yaws[total_counter] = graus;
							total_counter++;
						}
					}
				}
				else if(bfinderProxy->GetBlob(b).right == (camera_width) && bfinderProxy->GetBlob(b).left==0) // inacabado nas DUAS pontas.
				{
					if (blob_flag ==true)
					{
						if(blob_buffer.color == bfinderProxy->GetBlob(b).color)
						{
							blob_buffer = bfinderProxy->GetBlob(b);
						}
						else
						{
							//add the previous blob
							total_blobs_found[total_counter] = blob_buffer;
							total_yaws[total_counter] = yaw_buffer;
							total_counter++;
							yaw_buffer = graus;

						}

					}
					else
					{
						yaw_buffer = graus;
						blob_flag = true;
					}
				}
				else // ok, tudo certo :}
				{
					if (blob_flag ==true)
					{

						if(blob_buffer.color == buffer_color)
						{
							total_blobs_found[total_counter] =  bfinderProxy->GetBlob(b);
							total_yaws[total_counter] = graus;
							total_counter++;
							blob_flag = false;
						}
						else
						{
							//add the buffered blob.
							total_blobs_found[total_counter] = blob_buffer;
							total_yaws[total_counter] = yaw_buffer;
							total_counter++;

							blob_flag = false;
							total_blobs_found[total_counter] =  blob_buffer;
							total_yaws[total_counter] = graus;
							total_counter++;
						}

					}
					else
					{
						total_blobs_found[total_counter] =  bfinderProxy->GetBlob(b);
						total_yaws[total_counter] = graus;
						total_counter++;
					}
				}
			blob_buffer = bfinderProxy->GetBlob(b);
			}
		}

		graus +=30;
		// TODO gambiarra. deveria ter um método WaitUntilPlayed p aguardar o fim do audio
		sleep(2);
		head_yawi = head_yawi + 30; // more + 30 degree
		yaw_cnt++;


	}while (head_yawi < (90+30));

				//
	nro_blobs =0;
	//gambiarra
	for(int i=0; i<total_counter; i++)
	{
		if(color_str ==value_to_color(total_blobs_found[i].color))
		{
			_total_blobs_found[nro_blobs] = total_blobs_found[i];
			_total_yaws[nro_blobs] = total_yaws[i];
			nro_blobs++;
		}

	}

	// go back to the initial position
	headGoto(0);
	robot->ReadIfWaiting();




	scanText.imbue(loc);
	if (nro_blobs == 0)
	{
		scanText << string(translate("nenhum objeto encontrado com a cor ")) << color_str;
	}else 	if (nro_blobs == 1)
	{
		scanText << string(translate("1 objeto encontrado com a cor ")) << color_str;
	}else
	{
		scanText << nro_blobs << string(translate(" objetos encontrados com a cor ")) << color_str;
	}

	speak(scanText.str());
	// TODO gambiarra. deveria ter um método WaitUntilPlayed p aguardar o fim do audio
	//sleep(2);
	scanText.str("");
	scanText.clear();



	return nro_blobs;
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
		//wait to complete speech (9 chars/seg)
		sleep(text.length()/9.0);
	}
}

void DonnieClient::muteTTS(bool m)
{
	muted = m;
}
