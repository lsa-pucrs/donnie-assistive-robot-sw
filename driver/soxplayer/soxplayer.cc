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
		PLAYER_ERROR("Could not read SOUND ");
		SetError(-1);
		return;
	}
	//TODO. amory. pq speech interface ? revisar esta msg. dar find por speech
	if (AddInterface(this->sound_addr)){
		PLAYER_ERROR("Could not add speech interface ");
		SetError(-1);
		return;
	}

	//init sox
	//assert(sox_init() == SOX_SUCCESS);
	if(sox_init() != SOX_SUCCESS){
		PLAYER_ERROR("Could not initialize sox");
		exit(1);
	}

}
Soxplayer::~Soxplayer(){
	sox_quit();
	puts("Sox Closed");
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device (ONLY WHEN A CLIENT CONECTS TO THE DRIVER).  Return 0 if things go well, and -1 otherwise.
int Soxplayer::MainSetup(){   
	puts("Soxplayer client has been connected");

	//assossiate the device with an interface
	/*this->bumper_dev = deviceTable->GetDevice(this->bumper_addr);
	if (!(this->bumper_dev)) return -1;
	if (this->bumper_dev->Subscribe(this->InQueue)){
		this->bumper_dev = NULL;
		return -1;
	}*/

	puts("Soxplayer driver ready");
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//Shutdown the device (occurs in each client shutdown)
void Soxplayer::MainQuit(){
	PLAYER_MSG0(0, "Soxplayer client has been disconnected...\n");
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
	//PLAYER_MSG0(0,"Msg Received in Driver Soxplayer");
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_SOUND_CMD_VALUES, sound_addr)){
		Play((reinterpret_cast<player_sound_cmd_t*>(data))->filename);
		return(0);
	}

	return(-1);
}


int Soxplayer::Play(char *fileAddr){
	//check if file exists
	if (!std::ifstream(fileAddr))
	{
		PLAYER_ERROR1("File[%s] do not exists!\n",fileAddr);
		//printf("\nError! File[%s] do not exists!\n",fileAddr);
		return 1;
	}
	printf("Playing[%s]\n",fileAddr);

	static sox_format_t * in, * out; /* input and output files */
	sox_effects_chain_t * chain;
	sox_effect_t * e;
	sox_signalinfo_t interm_signal;
	char * args[10];

	/*if (argc != 2) {
	    fprintf(stderr, "usage: %s input_file \nIt supports any audio format supported by sox\n", argv[0]);
	    exit(1);
	}  */

	//assert(in = sox_open_read(fileAddr, NULL, NULL, NULL) == SOX_SUCCESS);
	
	if((in = sox_open_read(fileAddr, NULL, NULL, NULL)) == NULL){
		printf ("ERROR: cannot read audio file %s \n", fileAddr);
		exit(1);
	}
	
	/* Change "alsa" in this line to use an alternative audio device driver: */
	//assert(out= sox_open_write("default", &in->signal, NULL, "alsa", NULL, NULL) == SOX_SUCCESS );
	
	if((out= sox_open_write("default", &in->signal, NULL, "alsa", NULL, NULL)) == NULL){
		printf ("ERROR: cannot write audio device 'alsa' \n");
		exit(1);
		}

	chain = sox_create_effects_chain(&in->encoding, &out->encoding);

	interm_signal = in->signal; /* NB: deep copy */

	e = sox_create_effect(sox_find_effect("input"));
	//args[0] = (char *)in, assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
	args[0] = (char *)in, sox_effect_options(e, 1, args);
	//assert(sox_add_effect(chain, e, &interm_signal, &in->signal) == SOX_SUCCESS);
	sox_add_effect(chain, e, &interm_signal, &in->signal);
	free(e);
	
	printf ("rate %d %d\n", (int)in->signal.rate, (int)out->signal.rate);
	if (in->signal.rate != out->signal.rate) {
		e = sox_create_effect(sox_find_effect("rate"));
		//args[0] = "48000", assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
		//assert(sox_effect_options(e, 0, NULL) == SOX_SUCCESS);
		sox_effect_options(e, 0, NULL);
		//assert(sox_add_effect(chain, e, &interm_signal, &out->signal) == SOX_SUCCESS);
		sox_add_effect(chain, e, &interm_signal, &out->signal);
		free(e);

	}

	printf ("channels %d %d\n", in->signal.channels, out->signal.channels);
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
		puts("Soxplayer driver initiallized");
		Soxplayer_Register(table);
		puts("waiting for client startup...");                               //
		return(0);                                                           //
	}                                                                        //
}                                                                            //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
