/*
* Desc: A robot Soxplayer. Player for many Audio files formats.
* Author: Guilherme Marques - 
* Date:  August 2016
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
	name "soxplayer"
	plugin "libsoxplayer"
	provides ["sound:0"]
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

#include "sox.h" //to play audio files
//#include <assert.h>

#include <fstream> //to check file exists

#define DRIVERNAME "soxplayer"

// Message levels
#define MESSAGE_ERROR	0
#define MESSAGE_INFO	1
#define MESSAGE_DEBUG	2

////////////////////////////////////////////////////////////////////////////////
// The class for the driver
// devices are used to communication between drivers
// interfaces used to communication between client and the driver
class Soxplayer : public ThreadedDriver{
	public:
		Soxplayer(ConfigFile* cf, int section);
		~Soxplayer();
		virtual int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data);

	private:
		player_devaddr_t sound_addr;  //speech interface
		virtual int MainSetup();
		virtual void MainQuit();
		virtual void Main();

		virtual int  Play(char *fileAdr);
};

// Deal with assosiations comming by .cfg file
Soxplayer::Soxplayer(ConfigFile* cf, int section) : ThreadedDriver(cf, section){

	//create a sound interface
	if (cf->ReadDeviceAddr(&(this->sound_addr), section, "provides", PLAYER_SOUND_CODE, -1, NULL)){
		PLAYER_ERROR("[soxplayer] Could not read SOUND");
		SetError(-1);
		return;
	}
	//TODO. amory. pq speech interface ? revisar esta msg. dar find por speech
	if (AddInterface(this->sound_addr)){
		PLAYER_ERROR("[soxplayer] Could not add sox sound interface");
		SetError(-1);
		return;
	}

	//init sox
	//assert(sox_init() == SOX_SUCCESS);
	if(sox_init() != SOX_SUCCESS){
		PLAYER_ERROR("[soxplayer] Could not initialize sox");
		SetError(-1);
	}

}
Soxplayer::~Soxplayer(){
	sox_quit();
	PLAYER_MSG0(MESSAGE_INFO, "[soxplayer] Closed");
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device (ONLY WHEN A CLIENT CONECTS TO THE DRIVER).  Return 0 if things go well, and -1 otherwise.
int Soxplayer::MainSetup(){   
	PLAYER_MSG0(MESSAGE_INFO, "[soxplayer] client has been connected");

	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//Shutdown the device (occurs in each client shutdown)
void Soxplayer::MainQuit(){
	PLAYER_MSG0(MESSAGE_INFO, "[soxplayer] client has been disconnected...");
}

////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void Soxplayer::Main(){
	for(;;){
		pthread_testcancel();
		this->ProcessMessages();
		usleep(1000);
	}
	return;
}


//deal with messages comming from the clients
int Soxplayer::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data){
	#ifndef NDEBUG
	  PLAYER_MSG0(MESSAGE_INFO,"[soxplayer] Msg received");
	#endif
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_SOUND_CMD_VALUES, sound_addr)){
		Play((reinterpret_cast<player_sound_cmd_t*>(data))->filename);
		return(0);
	}

	return(-1);
}


int Soxplayer::Play(char *fileAddr){
	//check if file exists
	std::ifstream infile(fileAddr, std::ifstream::ate | std::ifstream::binary);
	if (!infile)
	{
		PLAYER_ERROR1("[soxplayer] File[%s] do not exists!",fileAddr);
		return -1;
	}
	// check if file is empty
    if (infile.tellg()<=0)	
	{
		PLAYER_ERROR1("[soxplayer] File[%s] is empty!",fileAddr);
		return -1;
	}
	infile.close();
 
	#ifndef NDEBUG
	  PLAYER_MSG1(MESSAGE_INFO, "[soxplayer] Playing[%s]",fileAddr);
	#endif

	static sox_format_t * in, * out; /* input and output files */
	sox_effects_chain_t * chain;
	sox_effect_t * e;
	sox_signalinfo_t interm_signal;
	char * args[10];

	//assert(in = sox_open_read(fileAddr, NULL, NULL, NULL) == SOX_SUCCESS);
	if((in = sox_open_read(fileAddr, NULL, NULL, NULL)) == NULL){
		PLAYER_ERROR1("[soxplayer] Cannot read audio file %s", fileAddr);
		return -1;
	}
	
	/* Change "alsa" in this line to use an alternative audio device driver: */
	//assert(out= sox_open_write("default", &in->signal, NULL, "alsa", NULL, NULL) == SOX_SUCCESS );
	if((out= sox_open_write("default", &in->signal, NULL, "alsa", NULL, NULL)) == NULL){
		PLAYER_ERROR("[soxplayer] cannot write audio device 'alsa'");
		return -1;
	}

	chain = sox_create_effects_chain(&in->encoding, &out->encoding);

	interm_signal = in->signal; /* NB: deep copy */

	e = sox_create_effect(sox_find_effect("input"));
	//args[0] = (char *)in, assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
	args[0] = (char *)in, sox_effect_options(e, 1, args);
	//assert(sox_add_effect(chain, e, &interm_signal, &in->signal) == SOX_SUCCESS);
	sox_add_effect(chain, e, &interm_signal, &in->signal);
	free(e);
	
	#ifndef NDEBUG
	  PLAYER_MSG2(MESSAGE_INFO, "[soxplayer] rate %d %d", (int)in->signal.rate, (int)out->signal.rate);
	#endif
	if (in->signal.rate != out->signal.rate) {
		e = sox_create_effect(sox_find_effect("rate"));
		//args[0] = "48000", assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
		//assert(sox_effect_options(e, 0, NULL) == SOX_SUCCESS);
		sox_effect_options(e, 0, NULL);
		//assert(sox_add_effect(chain, e, &interm_signal, &out->signal) == SOX_SUCCESS);
		sox_add_effect(chain, e, &interm_signal, &out->signal);
		free(e);
	}

    #ifndef NDEBUG
	  PLAYER_MSG2(MESSAGE_INFO, "[soxplayer] channels %d %d", in->signal.channels, out->signal.channels);
	#endif
	if (in->signal.channels != out->signal.channels) {
		e = sox_create_effect(sox_find_effect("channels"));
		//assert(sox_effect_options(e, 0, NULL) == SOX_SUCCESS);
		//assert(sox_add_effect(chain, e, &interm_signal, &out->signal) == SOX_SUCCESS);
		sox_effect_options(e, 0, NULL);
		sox_add_effect(chain, e, &interm_signal, &out->signal);
		free(e);
	}

	e = sox_create_effect(sox_find_effect("output"));
	//args[0] = (char *)out, assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
	//assert(sox_add_effect(chain, e, &interm_signal, &out->signal) == SOX_SUCCESS);
	args[0] = (char *)out, sox_effect_options(e, 1, args) ;
	sox_add_effect(chain, e, &interm_signal, &out->signal) ;
	free(e);

	sox_flow_effects(chain, NULL, NULL);
	sox_delete_effects_chain(chain);
	sox_close(out);
	sox_close(in);

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// A factory creation function, declared outside of the class so that it     //
// can be invoked without any object context (alternatively, you can         //
// declare it static in the class).  In this function, we create and return  //
// (as a generic Driver*) a pointer to a new instance of this driver.        //
Driver* Soxplayer_Init(ConfigFile* cf, int section){                            //
	// Create and return a new instance of this driver                       //
	return ((Driver*) (new Soxplayer(cf, section)));
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
		PLAYER_MSG0(MESSAGE_INFO, "[soxplayer] driver initiallized");
		Soxplayer_Register(table);
		PLAYER_MSG0(MESSAGE_INFO, "[soxplayer] waiting for client startup...");
		return(0);                                                           //
	}                                                                        //
}                                                                            //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
