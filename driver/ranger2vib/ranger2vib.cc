/*
* Desc: vibration belt drinver
* Author: Alexandre Amory
* Date:  Out 2018
* Laboratório de Sistemas Autônomos 
*  - https://lsa.pucrs.br/
*  - https://github.com/lsa-pucrs
* Faculdade de Informática - PUCRS  
*  - www.inf.pucrs.br
*/


/*
cfg example:
driver
(
	name "ranger2vib"
	plugin "libvib_belt"
	provides ["speech:0"]  #One can use the SpeechProxy.Say to set all vib motors at once
	provides ["dio:0"]     #One can use the DioProxy.SetOuput(val,idx) to set a single vib motor
)
*/

// TODO: separar a definicao da classe em um .h para gerar documentacao automatica com doxygen

#include <libplayercore/playercore.h>

#include <iostream>
#include <sys/time.h>
#include <cmath>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //sleep

#include <fstream> //to check file exists

using namespace std;

#define DRIVERNAME "ranger2vib"

// Message levels
#define MESSAGE_ERROR	0
#define MESSAGE_INFO	1
#define MESSAGE_DEBUG	2


// distance in meters (to be confirmed!!!)
#define INTENSE_VIBRATION_DISTANCE 1.0 // distances lower than this will cause intense vibration
#define MILD_VIBRATION_DISTANCE 1.5    // distances between INTENSE_VIBRATION_DISTANCE and MILD_VIBRATION_DISTANCE will cause mild vibration
// distances above MILD_VIBRATION_DISTANCE will cause no vibration

////////////////////////////////////////////////////////////////////////////////
// The class for the driver
// devices are used to communication between drivers
// interfaces used to communication between client and the driver
class Ranger2vib : public ThreadedDriver{
	public:
		Ranger2vib(ConfigFile* cf, int section);
		~Ranger2vib();
		virtual int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data);

	private:
		player_devaddr_t speech_addr;  //speech interface
		player_devaddr_t ranger_addr;  //dio interface
		//player_devaddr_t opaque_addr;  //dio interface
		
		Device * speech_dev;
		Device * ranger_dev;
		
		player_speech_cmd_t speech_data;

		// set 1 to enable debug mode
		uint8_t debug;
		// the content sent to the serial port
		char P[37];	
			
		virtual int MainSetup();
		virtual void MainQuit();
		virtual void Main();

		int Send(player_ranger_data_range_t *range);
};

// Deal with assosiations comming by .cfg file
Ranger2vib::Ranger2vib(ConfigFile* cf, int section) : ThreadedDriver(cf, section){
    memset(&(this->ranger_addr), 0, sizeof(player_devaddr_t));
    memset(&(this->speech_addr), 0, sizeof(player_devaddr_t));
    //memset(&(this->opaque_addr), 0, sizeof(player_devaddr_t));
    memset(&speech_data,0,sizeof(speech_data));
    speech_data.string = new char[40];

    // initialize P array
    this->P[0] = 'Z';
    for (int i = 1; i < 36; i++)
        this->P[i] = 'P';
  	// null terminated string
	P[36] = 0;
/*
	//create a speech interface
	if (cf->ReadDeviceAddr(&(this->opaque_addr), section, "provides", PLAYER_OPAQUE_CODE, -1, NULL)){
		PLAYER_ERROR("[ranger2vib] Could not read opaque");
		SetError(-1);
		return;
	}
	if (AddInterface(this->opaque_addr)){
		PLAYER_ERROR("[ranger2vib] Could not add opaque interface");
		SetError(-1);
		return;
	}
*/	
	//create a speech interface
	if (cf->ReadDeviceAddr(&(this->speech_addr), section, "provides", PLAYER_SPEECH_CODE, -1, NULL)){
		PLAYER_ERROR("[ranger2vib] Could not read speech");
		SetError(-1);
		return;
	}
	if (AddInterface(this->speech_addr)){
		PLAYER_ERROR("[ranger2vib] Could not add speech interface");
		SetError(-1);
		return;
	}	

	if (cf->ReadDeviceAddr(&(this->ranger_addr), section, "requires", PLAYER_RANGER_CODE, -1, NULL)){
		PLAYER_ERROR("[ranger2vib] Could not read ranger");
		SetError(-1);
		return;
	}

	debug = cf->ReadInt(section, "debug",0 );
}

Ranger2vib::~Ranger2vib(){
	PLAYER_MSG0(MESSAGE_INFO, "[ranger2vib] Closed");
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device (ONLY WHEN A CLIENT CONECTS TO THE DRIVER).  Return 0 if things go well, and -1 otherwise.
int Ranger2vib::MainSetup(){

    // Retrieve the handle to the speech device.
    this->speech_dev = deviceTable->GetDevice(this->speech_addr);
    if (!(this->speech_dev))
    {
      PLAYER_ERROR("[ranger2vib] unable to locate suitable speech device");
      return -1;
    }
    // Subscribe my message queue the speech device.
    if (this->speech_dev->Subscribe(this->InQueue))
    {
      PLAYER_ERROR("[ranger2vib] unable to subscribe to speech device");
      return -1;
    }	

    // Retrieve the handle to the ranger device.
    this->ranger_dev = deviceTable->GetDevice(this->ranger_addr);
    if (!(this->ranger_dev))
    {
      PLAYER_ERROR("[ranger2vib] unable to locate suitable ranger device");
      return -1;
    }
    // Subscribe my message queue the speech device.
    if (this->ranger_dev->Subscribe(this->InQueue))
    {
      PLAYER_ERROR("[ranger2vib] unable to subscribe to ranger device");
      return -1;
    }	
        	
	PLAYER_MSG0(MESSAGE_INFO, "[ranger2vib] client has been connected");
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//Shutdown the device (occurs in each client shutdown)
void Ranger2vib::MainQuit(){
	
	delete speech_data.string;
    this->speech_dev->Unsubscribe(this->InQueue);
    this->ranger_dev->Unsubscribe(this->InQueue);		
	PLAYER_MSG0(MESSAGE_INFO, "[ranger2vib] client has been disconnected...");
}

////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void Ranger2vib::Main(){
	for(;;){
		pthread_testcancel();
		this->ProcessMessages();
		usleep(1000);
	}
	return;
}


//deal with messages comming from the clients
int Ranger2vib::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data){
	if (this->debug == 1){
	  PLAYER_MSG2(MESSAGE_INFO,"[ranger2vib] Msg received. type %d - %d", hdr->type, hdr->subtype);
    }
    
    //dio msgs have more priority if both interfaces are used
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_RANGER_DATA_RANGE, ranger_addr)){
		if (this->debug == 1){
		  PLAYER_MSG0(MESSAGE_INFO,"[ranger2vib] Msg received via the dio interface.");
		}
		Send(reinterpret_cast<player_ranger_data_range_t*>(data));
		return(0);
	}else {
		if (this->debug == 1)
			PLAYER_MSG0(MESSAGE_INFO,"[ranger2vib] Msg ignored");
		return(0);		
	}

	return(-1);
}


int Ranger2vib::Send(player_ranger_data_range_t *range){
	
	speech_data.string_count = 36;
	
	// fill the speech data based on the distances of the rangers
    for(int i = 0; i < range->ranges_count; i++) {  //print all lasers receiveds
    
        // top line of vib motors . A is for critical distance (intense vib) and B is for mild vib. P stops vibrations
        if (range->ranges[i] < INTENSE_VIBRATION_DISTANCE)
            this->P[i+8] = 'A';
        else if(range->ranges[i] < MILD_VIBRATION_DISTANCE)
            this->P[i+8] = 'B';
        else
            this->P[i+8] = 'P';
/*
		// bottom line of vib motors - these are disabled for now
        if (this->myranger->GetRange(i) < INTENSE_VIBRATION_DISTANCE)
            this->P[i+22] = 'A';
        else if (this->myranger->GetRange(i) < MILD_VIBRATION_DISTANCE)
            this->P[i+22] = 'B';
        else
            this->P[i+22] = 'P';
    */
    }	

	strcpy(speech_data.string, this->P);
	
	// send to the serial port
	if (debug==1){
		cout << "[ranger2vib] printing the serial port content: " << speech_data.string << " " << strlen(speech_data.string) << endl  << endl;
	}	
	//send the textual description
	this->Publish(this->speech_addr,
                  PLAYER_MSGTYPE_DATA, PLAYER_SPEECH_CMD_SAY,
                  (void *)(&(this->speech_data)), sizeof(speech_data), NULL);
/*
	Publish (speech_addr, ret_queue, PLAYER_MSGTYPE_CMD, PLAYER_SPEECH_CMD_SAY, NULL, 0, NULL);
	this->speech_dev->PutMsg(this->InQueue, 
				  PLAYER_MSGTYPE_CMD, PLAYER_SPEECH_CMD_SAY, 
				  reinterpret_cast<void*>(&speech_data), sizeof(speech_data), NULL);    
*/
    /*
    if (write(this->fd, (void*)this->P, 36) == 0){
		cout << "[ranger2vib] error sending" << endl;
		// TODO uncomment this when serial port is actually sending data
		//return -1;
	}
	*/
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// A factory creation function, declared outside of the class so that it     //
// can be invoked without any object context (alternatively, you can         //
// declare it static in the class).  In this function, we create and return  //
// (as a generic Driver*) a pointer to a new instance of this driver.        //
Driver* Ranger2vib_Init(ConfigFile* cf, int section){                           //
	// Create and return a new instance of this driver                       //
	return ((Driver*) (new Ranger2vib(cf, section)));
}                                                                            //
// A driver registration function, again declared outside of the class so    //
// that it can be invoked without object context.  In this function, we add  //
// the driver into the given driver table, indicating which interface the    //
// driver can support and how to create a driver instance.                   //
void Ranger2vib_Register(DriverTable* table){                                   //
	table->AddDriver(DRIVERNAME, Ranger2vib_Init);							 //
}                                                                            //
// Extra stuff for building a shared object.                                 //
/* need the extern to avoid C++ name-mangling  */                            //
extern "C"{                                                                  //
	int player_driver_init(DriverTable* table){                              //
		PLAYER_MSG0(MESSAGE_INFO, "[ranger2vib] driver initiallized");
		Ranger2vib_Register(table);
		PLAYER_MSG0(MESSAGE_INFO, "[ranger2vib] waiting for client startup...");
		return(0);                                                           //
	}                                                                        //
}                                                                            //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
