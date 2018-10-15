/*
* Desc: This drivers allows a textual description of the map
* Author: Alexandre Amory
* Date:  Ouctober 2018
* Laboratório de Sistemas Autônomos 
*  - https://lsa.pucrs.br/
*  - https://github.com/lsa-pucrs
* Faculdade de Informática - PUCRS  
*  - www.pucrs.br
*/



/** @ingroup drivers */
/** @{ */
/** @defgroup driver_floorplan_description floorplan_description
 * @brief A driver to convert a room number to its textual description.

@par Provides

- @ref interface_log

@par Requires

- @ref interface_speech

@par Configuration requests

- None

@par Configuration file options

- pose (float tuple)
  - Default: [0.0 0.0 0.0]
  - Pose of the laser on the robot
- resolution (integer)
  - Default: 50 (which means 0.5 degree)
  - Angular scan resolution
- range_res (integer)
  - Default: 1
  - Range resolution
- occupied_value (float)
  - Default: 0.01
  - Value (in meters) returned by laser whenever bumper detects collision

@par Example

@verbatim
driver
(
	name "floorplan_description"
	plugin "libfloorplan_description"
	requires ["log:0"]   # selected to reuse a message player_log_power_config of type uint8_t
	provides ["speech:0"]  # textual output with description
	
	description "my little room"
)
@endverbatim

@author Alexandre Amory

*/

/** @} */



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

#define DRIVERNAME "floorplan_description"

// Message levels
#define MESSAGE_ERROR	0
#define MESSAGE_INFO	1
#define MESSAGE_DEBUG	2

// max description size
#define MAX_MSG_SIZE 1000

////////////////////////////////////////////////////////////////////////////////
// The class for the driver
// devices are used to communication between drivers
// interfaces used to communication between client and the driver
class FloorplanDescription : public ThreadedDriver{
	public:
		FloorplanDescription(ConfigFile* cf, int section);
		~FloorplanDescription();
		virtual int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data);

	private:
		player_devaddr_t speech_addr;  //speech interface
		// log has been selected to reuse a message player_log_set_write_state_t of type uint8_t
		player_devaddr_t log_addr;   //log interface
		

		player_speech_cmd_t speech_data;
			
        // Handle for the device that have the address given above
        Device * speech_dev;		
		
		//player_log_set_write_state_t log_data;
		//textual description
		std::string description_str;
		// file with the description. 
		// TODO: It could read an YAML file 
		std::string desc_filename_str;
		// set 1 to enable debug mode
		uint8_t debug;

		virtual int MainSetup();
		virtual void MainQuit();
		virtual void Main();

		virtual int  Play(uint8_t room_description);
};

// Deal with .cfg file
FloorplanDescription::FloorplanDescription(ConfigFile* cf, int section) : ThreadedDriver(cf, section){

    memset(&(this->log_addr), 0, sizeof(player_devaddr_t));
    memset(&(this->speech_addr), 0, sizeof(player_devaddr_t));
    memset(&speech_data,0,sizeof(speech_data));
    speech_data.string = new char[MAX_MSG_SIZE];

	//create the interface to request the room number
	if (cf->ReadDeviceAddr(&(this->log_addr), section, "provides", PLAYER_LOG_CODE, -1, NULL)){
		PLAYER_ERROR("[floorplan_description] Could not read log");
		SetError(-1);
		return;
	}
	// all new 'provides' interface must be registered
	if (AddInterface(this->log_addr)){
		PLAYER_ERROR("[floorplan_description] Could not add log interface");
		SetError(-1);
		return;
	}

	//create the interface to generate the textual description of the requested room
	if (cf->ReadDeviceAddr(&(this->speech_addr), section, "requires", PLAYER_SPEECH_CODE, -1, NULL)){
		PLAYER_ERROR("[floorplan_description] Could not read speech");
		SetError(-1);
		return;
	}

	debug = cf->ReadInt(section, "debug",0 );
	description_str = cf->ReadString (section, "description", "");
	desc_filename_str = cf->ReadString (section, "description_filename", "");
	//these two should be mutually exclusive. use one or the other method.
	// open the file here an read the description
	if((description_str == "" && desc_filename_str =="")){
		PLAYER_ERROR("[floorplan_description] Enter with description or description_filename field");
		SetError(-1);
		return;
	}
	if((description_str != "" && desc_filename_str !="")){
		PLAYER_ERROR("[floorplan_description] Can only use description or description_filename fields. not both at the same time");
		SetError(-1);
		return;
	}
	// TODO try to open the file. error if it is not opened
}
FloorplanDescription::~FloorplanDescription(){
	
	PLAYER_MSG0(MESSAGE_INFO, "[floorplan_description] Closed");
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device (ONLY WHEN A CLIENT CONECTS TO THE DRIVER).  Return 0 if things go well, and -1 otherwise.
int FloorplanDescription::MainSetup(){  
	
	//TODO read the desc_filename_str
	
    // Retrieve the handle to the speech device.
    this->speech_dev = deviceTable->GetDevice(this->speech_addr);
    if (!(this->speech_dev))
    {
      PLAYER_ERROR("[floorplan_description] unable to locate suitable speech device");
      return -1;
    }
    // Subscribe my message queue the speech device.
    if (this->speech_dev->Subscribe(this->InQueue))
    {
      PLAYER_ERROR("[floorplan_description] unable to subscribe to speech device");
      return -1;
    }	
	PLAYER_MSG0(MESSAGE_INFO, "[floorplan_description] client has been connected");
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//Shutdown the device (occurs in each client shutdown)
void FloorplanDescription::MainQuit(){
    // Unsubscribe from the speech
    delete speech_data.string;
    this->speech_dev->Unsubscribe(this->InQueue);	
	PLAYER_MSG0(MESSAGE_INFO, "[floorplan_description] client has been disconnected...");
}

////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void FloorplanDescription::Main(){
	for(;;){
		pthread_testcancel();
		this->ProcessMessages();
		usleep(1000);
	}
	return;
}


//deal with messages comming from the clients
int FloorplanDescription::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data){
	if (this->debug == 1){
	  PLAYER_MSG2(MESSAGE_INFO,"[floorplan_description] Msg received. type %d - %d", hdr->type, hdr->subtype);
    }
	
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_LOG_REQ_SET_WRITE_STATE, log_addr)){
		PLAYER_MSG1(MESSAGE_INFO,"[floorplan_description] Req msg %d", (reinterpret_cast<player_log_set_write_state_t*>(data))->state);
		Play((reinterpret_cast<player_log_set_write_state_t*>(data))->state);
		return(0);
	}else {
		if (this->debug == 1)
			PLAYER_MSG1(MESSAGE_INFO,"[floorplan_description] Ignored msg %d", (reinterpret_cast<player_log_set_write_state_t*>(data))->state);
		return(0);
	}
	
	if (this->debug == 1)
	  PLAYER_MSG0(MESSAGE_INFO,"[floorplan_description] shit ! wrong message !");
	return(-1);
}


int FloorplanDescription::Play(uint8_t room_description){

	speech_data.string_count = description_str.size();
	if (speech_data.string_count >= MAX_MSG_SIZE)
	{
		PLAYER_ERROR("Message too long");
		this->SetError(-1);
		return -1;
	}
	//TODO place here the logic to select the description text based on the room_description number. 0 means read it all.
	strcpy(speech_data.string, this->description_str.c_str());

	if (this->debug == 1)
	  PLAYER_MSG3(MESSAGE_INFO,"[floorplan_description] Sending sending message of room %d described as \"%s\" of %d bytes",room_description, speech_data.string, speech_data.string_count);

	//send the textual description
	this->speech_dev->PutMsg(this->InQueue, 
				  PLAYER_MSGTYPE_CMD, PLAYER_SPEECH_CMD_SAY, 
				  reinterpret_cast<void*>(&speech_data), sizeof(speech_data), NULL);

	return -1;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// A factory creation function, declared outside of the class so that it     //
// can be invoked without any object context (alternatively, you can         //
// declare it static in the class).  In this function, we create and return  //
// (as a generic Driver*) a pointer to a new instance of this driver.        //
Driver* Soxplayer_Init(ConfigFile* cf, int section){                            //
	// Create and return a new instance of this driver                       //
	return ((Driver*) (new FloorplanDescription(cf, section)));
}                                                                            //
// A driver registration function, again declared outside of the class so    //
// that it can be invoked without object context.  In this function, we add  //
// the driver into the given driver table, indicating which interface the    //
// driver can support and how to create a driver instance.                   //
void Soxplayer_Register(DriverTable* table){                                    //                                  //
	table->AddDriver(DRIVERNAME, Soxplayer_Init);
}                                                                            //
// Extra stuff for building a shared object.                                 //
/* need the extern to avoid C++ name-mangling  */                            //
extern "C"{                                                                  //
	int player_driver_init(DriverTable* table){                              //
		PLAYER_MSG0(MESSAGE_INFO, "[floorplan_description] driver initiallized");
		Soxplayer_Register(table);
		PLAYER_MSG0(MESSAGE_INFO, "[floorplan_description] waiting for client startup...");
		return(0);                                                           //
	}                                                                        //
}                                                                            //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
