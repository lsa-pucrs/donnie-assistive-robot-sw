#include "alerts.h"

// TODO: amory. criar include donnie_defs.h com todas as definicoes de tamanho do donnie
#define STEP_YAW 10 //gradianos
#define STEP_LENGHT 0.05
#define STEP_LENGHT_ERROR 0.03  //!Define o erro do ultimo passo. Caso o robo pare sem marcar o som de passo
#define SIDE_RANGER 0.05
#define FRONT_RANGER 0.06
#define BACK_RANGER 0.05
#define SCAN_YAW 30 //gradianos


DonnieClient::DonnieClient()
{
    string host = GetEnv("DONNIE_IP");
    string donnie_path = GetEnv("DONNIE_PATH");

    // Set up language environment
    generator gen;
    gen.add_messages_path(string(GetEnv("DONNIE_SOURCE_PATH")) + "/loc");
    gen.add_messages_domain("alerts");
    locale loc = gen(string(GetEnv("DONNIE_LANG")) + ".UTF-8");
    locale::global(loc);
    cout.imbue(loc);
    cerr.imbue(loc);

    int port = atoi(GetEnv("DONNIE_PORT").c_str());
    if(host.size()==0) host = "localhost";
    if(port==0) port = 6665;
    if (donnie_path=="") {
		cerr << translate("variable DONNIE_PATH not defined. Please execute 'export DONNIE_PATH=<path-to-donnie>'") << endl;
		exit(1);
	}

    // TODO: read a configuration file so user can change these sounds
	SSTEP   = donnie_path+"/resources/sounds/HIT.wav"; //Sound STEP
	SSBACK  = donnie_path+"/resources/sounds/192805sound13.wav"; //Sound Step BACK
	STRIGHT = donnie_path+"/resources/sounds/126413specialcoin.wav"; //Sound Turn RIGHT
	STLEFT  = donnie_path+"/resources/sounds/126412normalcoin.wav"; //Sound Turn LEFT
	SBUMPER = donnie_path+"/resources/sounds/mgs4_soundalert.wav"; //Sound Turn LEFT
	SRANGER = donnie_path+"/resources/sounds/mgs4_soundalert.wav"; //Sound Turn LEFT
    SSCAN   = donnie_path+"/resources/sounds/0-scan.wav"; //Sound Scan



	try{		
		robot = new PlayerClient(host,port);
		p2d = new Position2dProxy(robot,0);
		p2dhead = new Position2dProxy(robot,1);
		bumper = new BumperProxy(robot,0);
		ranger = new RangerProxy(robot,0);
		sound = new SoundProxy(robot, 0);
		//speech = new SpeechProxy(robot,0);
	}catch (PlayerError e){
		#ifndef NDEBUG
			cerr << e << endl;
		#endif
		cerr << translate("Não foi possível conectar no robô ") << endl;
		cerr << translate("Possivelmente o arquivo cfg está incorreto.") << endl;
		exit(1);
	}


    setPos(p2d->GetXPos(),p2d->GetYPos(),p2d->GetYaw());
    translation = 0;
    translationError = 0;
    rotation = 0;
    rotationError=0;
    alertBumperFlag = 0;
    alertRangerFlag = 0;
    robot->StartThread(); //create an robot->Read() in a separated thread
}

pos_t DonnieClient::getPos(){
    return pos;
}

void DonnieClient::setPos(double x, double y, double a){
    pos.x=x;
    pos.y=y;
    pos.a=a;
}

void DonnieClient::checkSteps(){
    /*cout << "POS("
        << p2d->GetXPos() << ", "
        << p2d->GetYPos () << ", "
        << p2d->GetYaw() << ")" << endl;
    cout << "translation:" << translation << endl;
    cout << "=============:" << endl;*/

    //Translation
    if(p2d->GetXSpeed()!=0){
        translation = translationError + hypot(p2d->GetXPos() - pos.x, p2d->GetYPos() - pos.y);
        if(translation>=STEP_LENGHT){ 
        	if(p2d->GetXSpeed()>0)sound->play((char *)SSTEP.c_str());
        	if(p2d->GetXSpeed()<0)sound->play((char *)SSBACK.c_str());
        	translationError=translation-STEP_LENGHT; 
    		setPos(p2d->GetXPos(),p2d->GetYPos(),pos.a); //update pos
    	}
    }

    //Translation Error management. In case of not plaing the last step
    if(p2d->GetXSpeed()==0){
        if(translation>=STEP_LENGHT_ERROR){
			/*if(dir>0)sound->play((char *)SSTEP.c_str());
			if(dir<0)sound->play((char *)SSBACK.c_str());*/
            #ifndef NDEBUG
            cout << translate("translation>=STEP_LENGHT_ERROR:") << translation << endl;
            #endif
        }
        translation=0;
    	translationError=0;
    	setPos(p2d->GetXPos(),p2d->GetYPos(),pos.a); //update pos
    }

    //Rotation
    if(p2d->GetYawSpeed()>0){
        rotation = p2d->GetYaw() - pos.a; //rotationError
        if(rotation>0 && rotation>M_PI) rotation=rotation-2*M_PI; //ajusta o valor do rotation 
        if(rotation<0 && rotation<-M_PI) rotation=rotation+2*M_PI;

        if(rotation>=DTOR(STEP_YAW)){ //-0.05 erro
            sound->play((char *)STLEFT.c_str());
            rotation = rotation - DTOR(STEP_YAW);
            setPos(pos.x,pos.y,p2d->GetYaw());
        }
    }
    if(p2d->GetYawSpeed()<0){
        rotation = p2d->GetYaw() - pos.a; //rotationError
        if(rotation>0 && rotation>M_PI) rotation=rotation-2*M_PI; //ajusta o valor do rotation 
        if(rotation<0 && rotation<-M_PI) rotation=rotation+2*M_PI;

        if(rotation<=-1*DTOR(STEP_YAW)){
            sound->play((char *)STRIGHT.c_str());
            rotation = rotation + DTOR(STEP_YAW);
            setPos(pos.x,pos.y,p2d->GetYaw());
        }
    }

    //Rotation Error management
    if(p2d->GetYawSpeed()==0){
        if(rotation>=DTOR(2))sound->play((char *)STLEFT.c_str()); //se tiver qualquer coisa no rotation quando parar gera um som
        if(rotation<=-1*DTOR(2))sound->play((char *)STRIGHT.c_str());
        rotation=0;
        setPos(pos.x,pos.y,p2d->GetYaw());
    }
}

void DonnieClient::checkBumpers(){
    if(bumper->IsAnyBumped()){
        if(!alertBumperFlag){
            //cout << "BUMPED!:" << endl << endl;
            alertBumperFlag=1;
            sound->play((char *)SBUMPER.c_str());
            //speech->Say("O robô colidiu com alguma coisa!");
        }
    }
    else alertBumperFlag = 0;

}

void DonnieClient::checkRangers(){
    ///100 to convert from metters to cm
    if ((ranger->GetRange(0))<SIDE_RANGER || //fr 
        (ranger->GetRange(1))<FRONT_RANGER || //front
        (ranger->GetRange(2))<SIDE_RANGER || //fl
        (ranger->GetRange(3))<SIDE_RANGER || //bl
        (ranger->GetRange(4))<BACK_RANGER || //back
        (ranger->GetRange(5))<SIDE_RANGER){  //br
       
        if(!alertRangerFlag){
            //cout << "RANGED!:" << endl << endl;
            //for(int i=0;i<6;i++){
               //cout <<"ranger"<< i << ":" << (ranger->GetRange(i)) << endl;
            //}
            //cout << endl;
            alertRangerFlag = 1;
            sound->play((char *)SRANGER.c_str());
            //speech->Say("Existem obstáculos no caminho");
        }
    }
    else alertRangerFlag = 0; 
}


void DonnieClient::checkHead(){
    if(p2dhead->GetYawSpeed()!=0)
        sound->play((char *)SSCAN.c_str());
}

int main(int argc, char *argv[]){
    DonnieClient *donnie1 = new DonnieClient();
    while(1){
        usleep(100); //little delay
        donnie1->checkSteps();
        donnie1->checkBumpers();
        donnie1->checkRangers();
        donnie1->checkHead();
    }
    return 0;
}
