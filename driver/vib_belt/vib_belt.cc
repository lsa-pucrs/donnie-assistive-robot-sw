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
	name "vib_belt"
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

#include <termios.h>
#include <sys/signal.h>
#include <sys/types.h>

using namespace std;

#define DRIVERNAME "vib_belt"

// Message levels
#define MESSAGE_ERROR	0
#define MESSAGE_INFO	1
#define MESSAGE_DEBUG	2

#define BAUDRATE B9600

////////////////////////////////////////////////////////////////////////////////
// The class for the driver
// devices are used to communication between drivers
// interfaces used to communication between client and the driver
class Vibbelt : public ThreadedDriver{
	public:
		Vibbelt(ConfigFile* cf, int section);
		~Vibbelt();
		virtual int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data);

	private:
		player_devaddr_t speech_addr;  //speech interface
		player_devaddr_t dio_addr;     //dio interface
		
		player_speech_cmd_t speech_data;
		
		// set 1 to enable debug mode
		uint8_t debug;
		// serial port name
		string devicename;
		// serial port file descriptor
		int fd;
		// 1 when serial interface has been connected
		int status;
		// the content sent to the serial port
		char P[37];	
			
		virtual int MainSetup();
		virtual void MainQuit();
		virtual void Main();

		virtual int  Send();
		int Set(char *serial_str);
		int SetSingle(player_dio_cmd_t *dio);
};

// Deal with assosiations comming by .cfg file
Vibbelt::Vibbelt(ConfigFile* cf, int section) : ThreadedDriver(cf, section){
    memset(&(this->dio_addr), 0, sizeof(player_devaddr_t));
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

	//create a speech interface
	if (cf->ReadDeviceAddr(&(this->speech_addr), section, "provides", PLAYER_SPEECH_CODE, -1, NULL)){
		PLAYER_ERROR("[vib_belt] Could not read speech");
		SetError(-1);
		return;
	}
	if (AddInterface(this->speech_addr)){
		PLAYER_ERROR("[vib_belt] Could not add speech interface");
		SetError(-1);
		return;
	}
		
	if (cf->ReadDeviceAddr(&(this->dio_addr), section, "provides", PLAYER_DIO_CODE, -1, NULL)){
		PLAYER_ERROR("[vib_belt] Could not read dio");
		SetError(-1);
		return;
	}
	if (AddInterface(this->dio_addr)){
		PLAYER_ERROR("[vib_belt] Could not add dio interface");
		SetError(-1);
		return;
	}
		
	status = 0;
	debug = cf->ReadInt(section, "debug",0 );
	devicename = cf->ReadString (section, "serial_port", "");
	if (devicename == ""){
		PLAYER_ERROR("[vib_belt] serial_port field nor found.");
		SetError(-1);
		return;	
	}


}
Vibbelt::~Vibbelt(){
	PLAYER_MSG0(MESSAGE_INFO, "[vib_belt] Closed");
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device (ONLY WHEN A CLIENT CONECTS TO THE DRIVER).  Return 0 if things go well, and -1 otherwise.
int Vibbelt::MainSetup(){
	
	cout << "[vib_belt] connecting ..." << endl;	

	// TODO remove the 3 lines below when testing with the belt
	//status = 1;	
	//PLAYER_MSG0(MESSAGE_INFO, "[vib_belt] client has been connected");
	//return(0);	
	
    //this->fd = open(devicename.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    this->fd = open(devicename.c_str(), O_RDWR); 

    if (this->fd < 0){
		cout << "[vib_belt] error to open the serial port" << endl;
        return -1;
	}

	status = 1;	
	PLAYER_MSG0(MESSAGE_INFO, "[vib_belt] client has been connected");

	return(0);
        
	//serial setup
    int res, i;
    //struct terminos newtio;
    struct termios newtio;
    //struct sigaction saio;
    struct sigaction saio;
    char buf[255];

    // Install the serial handler before making the device
    saio.sa_handler     = NULL; // signal_handler_IO;
    sigemptyset(&saio.sa_mask); // saio.sa_mask = 0;
    saio.sa_flags       = 0;
    saio.sa_restorer    = NULL;
    sigaction(SIGIO, &saio, NULL);
    // Allow the process to receive SIGIO
    fcntl(this->
        fd, F_SETOWN, getpid());
    // Make the fil descriptor asynchronous
    fcntl(this->fd, F_SETFL, FASYNC);
    // Set new port settings for canonical input processing
    newtio.c_cflag      = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag      = IGNPAR;
    newtio.c_oflag      = 0;
    newtio.c_lflag      = 0;
    newtio.c_cc[VMIN]   = 1;
    newtio.c_cc[VTIME]  = 0;

    tcflush(this->fd, TCIFLUSH);
    tcsetattr(this->fd,TCSANOW,&newtio);

	status = 1;	
	PLAYER_MSG0(MESSAGE_INFO, "[vib_belt] client has been connected");

	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//Shutdown the device (occurs in each client shutdown)
void Vibbelt::MainQuit(){
	close(this->fd);
	delete speech_data.string;
    //this->speech_dev->Unsubscribe(this->InQueue);
    //this->dio_dev->Unsubscribe(this->InQueue);		
	PLAYER_MSG0(MESSAGE_INFO, "[vib_belt] client has been disconnected...");
}

////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void Vibbelt::Main(){
	for(;;){
		pthread_testcancel();
		this->ProcessMessages();
		//TODO chage here to increase the interval between serial messages, reducing the update frequency.
		usleep(500);
	}
	return;
}


//deal with messages comming from the clients
int Vibbelt::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data){
	if (this->debug == 1){
	  PLAYER_MSG2(MESSAGE_INFO,"[vib_belt] Msg received. type %d - %d", hdr->type, hdr->subtype);
    }
    
    //dio msgs have more priority if both interfaces are used
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_DIO_CMD_VALUES, dio_addr)){
		if (this->debug == 1){
		  PLAYER_MSG0(MESSAGE_INFO,"[vib_belt] Msg received via the dio interface.");
		}
		SetSingle(reinterpret_cast<player_dio_cmd_t*>(data));
		return(0);
	}else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_SPEECH_CMD_SAY, speech_addr)){
		if (this->debug == 1){
		  PLAYER_MSG0(MESSAGE_INFO,"[vib_belt] Msg received via the speech interface.");
		}
		Set((reinterpret_cast<player_speech_cmd_t*>(data))->string);
		return(0);
	}else {
		if (this->debug == 1)
			PLAYER_MSG0(MESSAGE_INFO,"[vib_belt] Msg ignored");
		return(0);		
	}

	return(-1);
}

int Vibbelt::SetSingle(player_dio_cmd_t *dio){
	
	if (!(dio->count < 36 && dio->count >= 0)){
	  PLAYER_MSG1(MESSAGE_INFO,"[vib_belt] Unexpected vibmotor id %d", dio->count);
	  return(-1);		
	}
	if (!(dio->digout <= 'Z' && dio->digout >= 'A')){
	  PLAYER_MSG1(MESSAGE_INFO,"[vib_belt] Unexpected vibmotor value %c", (char)dio->digout);
	  return(-1);		
	}	
	this->P[dio->count] = dio->digout;
	Send();
}

int Vibbelt::Send(){
	//check the serial connection
	if (status==0){
		cout << "[vib_belt] serial port not connected." << endl;
		return -1;
	}
	if (debug==1){
		cout << "[vib_belt] printing the serial port content: " << this->P << " " << strlen(this->P) << endl  << endl;
	}	
    // all writing operations are checked to see if they are working properly, in the case in which they aren't, the function will return false.
    // TODO uncomment this when testing serial
    if (write(this->fd, (void*)this->P, 36) == 0){
		cout << "[vib_belt] error sending" << endl;
		// TODO uncomment this when serial port is actually sending data
		return -1;
	}
	return 0;
	
}

int Vibbelt::Set(char *serial_str){
	
	if (strlen(serial_str)>36){
		PLAYER_MSG0(MESSAGE_INFO,"[vib_belt] string is too big");
		return -1;
	}
	strcpy(this->P,serial_str);
	// send to the serial port
	Send();
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// A factory creation function, declared outside of the class so that it     //
// can be invoked without any object context (alternatively, you can         //
// declare it static in the class).  In this function, we create and return  //
// (as a generic Driver*) a pointer to a new instance of this driver.        //
Driver* Vibbelt_Init(ConfigFile* cf, int section){                           //
	// Create and return a new instance of this driver                       //
	return ((Driver*) (new Vibbelt(cf, section)));
}                                                                            //
// A driver registration function, again declared outside of the class so    //
// that it can be invoked without object context.  In this function, we add  //
// the driver into the given driver table, indicating which interface the    //
// driver can support and how to create a driver instance.                   //
void Vibbelt_Register(DriverTable* table){                                   //
	table->AddDriver(DRIVERNAME, Vibbelt_Init);							 //
}                                                                            //
// Extra stuff for building a shared object.                                 //
/* need the extern to avoid C++ name-mangling  */                            //
extern "C"{                                                                  //
	int player_driver_init(DriverTable* table){                              //
		PLAYER_MSG0(MESSAGE_INFO, "[vib_belt] driver initiallized");
		Vibbelt_Register(table);
		PLAYER_MSG0(MESSAGE_INFO, "[vib_belt] waiting for client startup...");
		return(0);                                                           //
	}                                                                        //
}                                                                            //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
