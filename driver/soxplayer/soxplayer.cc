#include "soxplayer.h"
#include <libplayercore/playercore.h>
#include "sox.h" //to play audio files



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// A factory creation function, declared outside of the class so that it     //
// can be invoked without any object context (alternatively, you can         //
// declare it static in the class).  In this function, we create and return  //
// (as a generic Driver*) a pointer to a new instance of this driver.        //
Driver* Soxplayer_Init(ConfigFile* cf, int section){                         //
	// Create and return a new instance of this driver                       //
	return ((Driver*) (new Soxplayer(cf, section)));                         //
}                                                                            //
// A driver registration function, again declared outside of the class so    //
// that it can be invoked without object context.  In this function, we add  //
// the driver into the given driver table, indicating which interface the    //
// driver can support and how to create a driver instance.                   //
void Soxplayer_Register(DriverTable* table){                                 //
	table->AddDriver("soxplayer", Soxplayer_Init);                           //
}                                                                            //
// Extra stuff for building a shared object.                                 //
/* need the extern to avoid C++ name-mangling  */                            //
extern "C"{                                                                  //
	int player_driver_init(DriverTable* table){                              //
	  PLAYER_MSG0(MESSAGE_INFO, "[soxplayer] driver initiallized");          //
	  Soxplayer_Register(table);                                             //
	  PLAYER_MSG0(MESSAGE_INFO, "[soxplayer] waiting for client startup...");//
	  return(0);                                                             //
	}                                                                        //
}                                                                            //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Soxplayer::Soxplayer(ConfigFile* cf, int section) : ThreadedDriver(cf, section){

	//create a sound interface
	if (cf->ReadDeviceAddr(&(this->sound_addr), section, "provides", PLAYER_PLAYSOUND_CODE, -1, NULL)){
		PLAYER_ERROR("[soxplayer] Could not read SOUND");
		SetError(-1);
		return;
	}

	if (AddInterface(this->sound_addr)){
		PLAYER_ERROR("[soxplayer] Could not add sox sound interface");
		SetError(-1);
		return;
	}

	//init sox
	if(sox_init() != SOX_SUCCESS){
		PLAYER_ERROR("[soxplayer] Could not initialize sox");
		SetError(-1);
	}

	speed_str = cf->ReadString (section, "speed", "");
	pitch_str = cf->ReadString (section, "pitch", "");
	tempo_str = cf->ReadString (section, "tempo", "");
	
#ifndef NDEBUG
	  PLAYER_MSG1(MESSAGE_INFO, "[soxplayer] SPEED:  [%s]",speed_str);
	  PLAYER_MSG1(MESSAGE_INFO, "[soxplayer] PITCH:  [%s]",pitch_str);
	  PLAYER_MSG1(MESSAGE_INFO, "[soxplayer] TEMPO:  [%s]",tempo_str);
#endif
	
}

Soxplayer::~Soxplayer(){
	sox_quit();
	PLAYER_MSG0(MESSAGE_INFO, "[soxplayer] Closed");
}

int Soxplayer::MainSetup(){   
	PLAYER_MSG0(MESSAGE_INFO, "[soxplayer] client has been connected");

	return(0);
}

void Soxplayer::MainQuit(){
	PLAYER_MSG0(MESSAGE_INFO, "[soxplayer] client has been disconnected...");
}

void Soxplayer::Main(){
	for(;;){
		pthread_testcancel();
		this->ProcessMessages();
		usleep(1000);
	}
	return;
}

int Soxplayer::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data){
	#ifndef NDEBUG
	  PLAYER_MSG0(MESSAGE_INFO,"[soxplayer] Msg received");
	#endif
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_PLAYSOUND_CMD_VALUES, sound_addr)){
		Play((reinterpret_cast<player_playsound_cmd_t*>(data))->string);
		return(0);
	}

	return(-1);
}


int Soxplayer::Play(char *fileAddr){

	#ifndef NDEBUG
	  PLAYER_MSG1(MESSAGE_INFO, "[soxplayer] Playing [%s]",fileAddr);
	#endif

	//check if file exists
	std::ifstream infile(fileAddr, std::ifstream::ate | std::ifstream::binary);
	if (!infile)
	{
		PLAYER_ERROR1("[soxplayer] File[%s] does not exist!",fileAddr);
		return -1;
	}
	// check if file is empty
    if (infile.tellg()<=0)	
	{
		PLAYER_ERROR1("[soxplayer] File[%s] is empty!",fileAddr);
		return -1;
	}
	infile.close();
 
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

	//Create SPEED Effect
	if(!speed_str.empty()){
		e = sox_create_effect(sox_find_effect("speed"));
		//args[0] = "1.8" , sox_effect_options(e, 1, args);  //default 1; fast <1; slow >1  
		args[0] = (char *)speed_str.c_str(), sox_effect_options(e, 1, args);
		sox_add_effect(chain, e, &out->signal, &out->signal);
		free(e);
	}

	
	//Create PITCH Effect
	if(!pitch_str.empty()){
		e = sox_create_effect(sox_find_effect("pitch"));
		//args[0] = "1", sox_effect_options(e, 1, args);
		args[0] = (char *)pitch_str.c_str(), sox_effect_options(e, 1, args);
		sox_add_effect(chain, e,  &out->signal, &out->signal);
		free(e);		
	}
	
	////Create TEMPO Effect
	if(!tempo_str.empty()){
		e = sox_create_effect(sox_find_effect("tempo"));
		//args[0] = "0.6" , sox_effect_options(e, 1, args);  //default 1; fast >1; slow <1  
		args[0] = (char *)tempo_str.c_str(), sox_effect_options(e, 1, args);
		sox_add_effect(chain, e,  &out->signal, &out->signal);
		free(e);
	}

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

