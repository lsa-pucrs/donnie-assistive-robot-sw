#include "alerts.h"

// TODO: amory. criar include donnie_defs.h com todas as definicoes de tamanho do donnie
#define STEP_YAW 10 //gradianos
#define STEP_LENGHT 0.05  //TODO ver se esta certo com o client do augusto
#define SIDE_RANGER 0.05   //TODO ver se esta certo com o client do augusto
#define FRONT_RANGER 0.06   //TODO ver se esta certo com o client do augusto
#define BACK_RANGER 0.05  //TODO ver se esta certo com o client do augusto


DonnieClient::DonnieClient()
{
    string host = GetEnv("DONNIE_IP");
    string donnie_path = GetEnv("DONNIE_PATH");
    int port = atoi(GetEnv("DONNIE_PORT").c_str());
    if(host.size()==0) host = "localhost";
    if(port==0) port = 6665;
    if (donnie_path=="") {
		cerr << "variable DONNIE_PATH not defined. Please execute 'export DONNIE_PATH=<path-to-donnie>'" << endl;
		exit(1);
	}

	// TODO: read a configuration file so user can change these sounds
	SSTEP   = donnie_path+"/resources/sounds/HIT.wav"; //Sound STEP
	SSBACK  = donnie_path+"/resources/sounds/beep-01a.wav"; //Sound Step BACK
	STRIGHT = donnie_path+"/resources/sounds/right.wav"; //Sound Turn RIGHT
	STLEFT  = donnie_path+"/resources/sounds/left.wav"; //Sound Turn LEFT
	SBUMPER = donnie_path+"/resources/sounds/mgs4_soundalert.wav"; //Sound Turn LEFT
	SRANGER = donnie_path+"/resources/sounds/mgs4_soundalert.wav"; //Sound Turn LEFT


    robot = new PlayerClient(host,port);
    //head = new PlayerClient("localhost",6666);
    p2d = new Position2dProxy(robot,0);
    //p2d_headProxy = new Position2dProxy(robot,1);
    //actuator = new ActArrayProxy(robot,0);
    bumper = new BumperProxy(robot,0);
    //BfinderProxy = new BlobfinderProxy(head,0);
    ranger = new RangerProxy(robot,0);
    //SHProxy = new RangerProxy(head,3);
    sound = new SoundProxy(robot, 0);

    speech = new SpeechProxy(robot,0);


    setPos(p2d->GetXPos(),p2d->GetYPos(),p2d->GetYaw());
    double translation = 0;
    double rotation = 0;
    steps = 0;
    alertStepFlag = 0;
    alertBumperFlag = 0;
    alertRangerFlag = 0;
    robot->StartThread();
}

pos_t DonnieClient::getPos(){
    return pos;
}

void DonnieClient::setPos(double x, double y, double a){
    pos.x=x;
    pos.y=y;
    pos.a=a;
}

void DonnieClient::checkDir(){
    robot->ReadIfWaiting();


    /*
    cout << "VEL("
    << p2d->GetXSpeed() << ", "
    << p2d->GetYSpeed () << ", "
    << p2d->GetYawSpeed() << ")" << endl << endl;
    */

    translation = hypotf(p2d->GetXPos() - pos.x, p2d->GetYPos() - pos.y) - translation;
    //cout << "translation:" << translation << endl;
    //cout << "steps:" << steps << endl;
    
    if(translation>=STEP_LENGHT){  //if(translation>(steps+1)*STEP_LENGHT){
        if(p2d->GetXSpeed()>0){
            steps++;
            sound->play((char *)SSTEP.c_str());
            //cout << "forward:" << translation << endl;
            //cout << "steps:" << steps << endl << endl;
        }
        else if(p2d->GetXSpeed()<0){
            steps--;
            sound->play((char *)SSBACK.c_str());
            //cout << "backward:" << translation << endl;
            //cout << "steps:" << steps << endl << endl;
        }
        setPos(p2d->GetXPos(),p2d->GetYPos(),pos.y);
        translation=0; 
        //cout << "gtranslation:" << translation << endl;
        //cout << "gsteps:" << steps << endl;
    }


    //cout << "Yaw:" << radTOdeg(p2d->GetYaw()) << endl << endl;
    if(p2d->GetYawSpeed()>0){ //ge
        if(radTOdeg(p2d->GetYaw() - pos.a)>STEP_YAW){
            //cout << "ge Yaw-a:" << radTOdeg(p2d->GetYaw() - pos.a) << endl << endl;
            setPos(pos.x,pos.y,p2d->GetYaw());
            sound->play((char *)STLEFT.c_str());
        }
    }
    else if(p2d->GetYawSpeed()<0){ //gd
        if(radTOdeg(pos.a - p2d->GetYaw())>STEP_YAW){
            //cout << "gd Yaw-a:" << radTOdeg(pos.a - p2d->GetYaw()) << endl << endl;
            setPos(pos.x,pos.y,p2d->GetYaw());
            sound->play((char *)STRIGHT.c_str());
        }
    }
    

}

void DonnieClient::checkBumpers(){
    robot->ReadIfWaiting();

    if(bumper->IsAnyBumped()){
        if(!alertBumperFlag){
            cout << "BUMPED!:" << endl << endl;
            alertBumperFlag=1;
            sound->play((char *)SBUMPER.c_str());
            speech->Say(" Alguma coisa se colidiu com o robô\n");
        }
    }
    else alertBumperFlag = 0;
}

void DonnieClient::checkRangers(){
    robot->Read();
    ///100 to convert from metters to cm
    if ((ranger->GetRange(0))<SIDE_RANGER || //fr 
        (ranger->GetRange(1))<FRONT_RANGER || //front
        (ranger->GetRange(2))<SIDE_RANGER || //fl
        (ranger->GetRange(3))<SIDE_RANGER || //bl
        (ranger->GetRange(4))<BACK_RANGER || //back
        (ranger->GetRange(5))<SIDE_RANGER){  //br
       
        if(!alertRangerFlag){
            cout << "RANGED!:" << endl << endl;
            for(int i=0;i<6;i++){
               cout <<"ranger"<< i << ":" << (ranger->GetRange(i)) << endl;
            }
            cout << endl;
            alertRangerFlag = 1;
            sound->play((char *)SRANGER.c_str());
            //speech->Say(" Existem obstaculos no caminho\n");
        }
    }
    else alertRangerFlag = 0; 
}


int main(int argc, char *argv[]){
    DonnieClient *donnie1 = new DonnieClient();
    while(1){
        donnie1->checkDir();
        donnie1->checkBumpers();
        donnie1->checkRangers();
        //TODO : amory. Marques, acho q tem q colocar um sleep pequeno aqui pois vcs está fazendo requisições ao Read com muita frequencia
    }

    return 0;
}
