/*
 * Desc: A robot Gtts handle
 * Author: Guilherme Marques
 * Date:  August 2016
 */

/*
cfg example:
driver
(
	name "gtts"
	plugin "libgtts"
	requires ["sound:0"]
	provides ["speech:0"]
)

driver
(
	name "soxplayer"
	plugin "libsoxplayer"
	provides ["sound:0"]
)

*/



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
#include <curl/curl.h> 
#include <netinet/in.h> //TODO perguntar para henry oq isso faz

#include <assert.h>


#define DRIVERNAME "gtts"


CURL *curl;  
FILE *file;


////////////////////////////////////////////////////////////////////////////////
// The class for the driver
// devices are used to communication between drivers
// interfaces used to communication between client and the driver
class Gtts : public ThreadedDriver{
	public:
		Gtts(ConfigFile* cf, int section);
		virtual int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data);

	private:
		player_devaddr_t speech_addr;  //speech interface
		player_devaddr_t sound_addr;  //sound interface
		Device *sound_dev; //sound device

		virtual int MainSetup();
		virtual void MainQuit();
		virtual void Main();
		virtual void ProcessSpeechCmd(player_msghdr_t* hdr, player_speech_cmd_t &data);
		virtual void Play(char *fileAddr);
};

// Deal with assosiations comming by .cfg file
Gtts::Gtts(ConfigFile* cf, int section) : ThreadedDriver(cf, section){
	memset (&this->sound_addr, 0, sizeof (player_devaddr_t));
	this->sound_dev = NULL;

	//requires assosiations with devies
	if (cf->ReadDeviceAddr (&(this->sound_addr),section,"requires",PLAYER_SOUND_CODE, -1, NULL)){
		PLAYER_ERROR("Could not read sound interface ");
		SetError (-1);
		return;
	}
	//requires assosiation with interfaces
	if (cf->ReadDeviceAddr(&(this->speech_addr), section, "provides", PLAYER_SPEECH_CODE, -1, NULL)){
		PLAYER_ERROR("Could not read SPEECH ");
		SetError(-1);
		return;
	}
	if (AddInterface(this->speech_addr)){
		PLAYER_ERROR("Could not add speech interface ");
		SetError(-1);
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device (ONLY WHEN A CLIENT CONECTS TO THE DRIVER).  Return 0 if things go well, and -1 otherwise.
int Gtts::MainSetup(){   
	PLAYER_MSG0(0, "Gtts client has been connected\n");
	puts("Gtts driver initialising");

	//assossiate the device with an interface
	//Gtts this driver with Sound driver like a client does
	this->sound_dev = deviceTable->GetDevice(this->sound_addr);
	if (!(this->sound_dev)) return -1;
	if (this->sound_dev->Subscribe(this->InQueue)){
		this->sound_dev = NULL;
		return -1;
	}

	puts("Gtts driver ready");
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//Shutdown the device (occurs in each client shutdown)
void Gtts::MainQuit(){
	PLAYER_MSG0(0, "Gtts client has been disconnected...\n");
	//return 0; //close server
}

////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void Gtts::Main(){
	for(;;){
		pthread_testcancel();
		ProcessMessages();
		usleep(1000);
	}
	return;
}


//deal with messages comming from the clients
int Gtts::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data){
	//PLAYER_MSG0(0,"Msg Received in Driver Gtts");
	/*if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_SOUND_DATA_STATE, sound_addr)){
		Play(*reinterpret_cast<player_sound_data_t *>(data));
		//PLAYER_WARN("sound data received");
		return(0);
	}*/
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_SPEECH_CMD_SAY, speech_addr)){
		ProcessSpeechCmd(hdr, *reinterpret_cast<player_speech_cmd_t*>(data));
		return 0;
	}

	return(-1);
}

void Gtts::ProcessSpeechCmd(player_msghdr_t* hdr, player_speech_cmd_t &data){
	std::cout << std::endl << "Received:";
	for(int i=0;i<data.string_count;i++){
		std::cout << data.string[i];
	}
	std::cout << std::endl;

	char *palavra = data.string+1;
	//trata os espaços em branco da palavra ' '->%
	int i = 0;
	for(i = 0; i < data.string_count; i++)
	{
		if(*(palavra+i) == ' ')
		{
			*(palavra+i) = '+';
		}
	}
	//printf("%s\n", palavra);

	char url[1000];
	//FULL BASE URL strcpy(url,"http://translate.google.com/translate_tts?ie=UTF-8&total=1&idx=0&textlen=32&client=tw-ob&tl=PT-br&q=");
	strcpy(url,"http://translate.google.com/translate_tts?");
	strcat(url,"ie=UTF-8");
	strcat(url,"&total=1");
	strcat(url,"&idx=0");
	strcat(url,"&textlen=32");
	strcat(url,"&client=tw-ob");
	strcat(url,"&tl=PT-br");
	strcat(url,"&q=");
	strcat(url, palavra);
	//printf("%s\n", url);
	
	//Download do arquivo com curl
    //file = fopen("download.mp3", "w");
    //curl = curl_easy_init();
    //curl_easy_setopt(curl, CURLOPT_URL, url);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    //curl_easy_perform(curl);
    //curl_easy_cleanup(curl);
    //fclose(file);

    //terminal example
    //wget -q -U Mozilla -O output.mp3 "http://translate.google.com/translate_tts?ie=UTF-8&total=1&idx=0&textlen=32&client=tw-ob&tl=PT-br&q=+mundo+inteiro+é+um+palco"
    
    char cmdcurl[1000];
    strcpy(cmdcurl,"wget -q -U Mozilla -O download.mp3 \"");
    strcat(cmdcurl,url);
    strcat(cmdcurl,"\"");
    system(cmdcurl);
    //printf("%s\n", cmdcurl);
    Play("download.mp3");
}

void Gtts::Play(char *fileAddr){
	player_sound_cmd_t sfile;
	memset(&sfile,0,sizeof(sfile));

	strcpy(sfile.filename, fileAddr);
	//printf("Playing?[%s]\n",sfile.filename);

	//send sox command
	this->sound_dev->PutMsg(this->InQueue, 
				  PLAYER_MSGTYPE_CMD, PLAYER_SOUND_CMD_VALUES, 
				  reinterpret_cast<void*>(&sfile), sizeof(sfile), NULL);

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// A factory creation function, declared outside of the class so that it     //
// can be invoked without any object context (alternatively, you can         //
// declare it static in the class).  In this function, we create and return  //
// (as a generic Driver*) a pointer to a new instance of this driver.        //
Driver* Gtts_Init(ConfigFile* cf, int section){                            //
	// Create and return a new instance of this driver                       //
	return ((Driver*) (new Gtts(cf, section)));
}                                                                            //
// A driver registration function, again declared outside of the class so    //
// that it can be invoked without object context.  In this function, we add  //
// the driver into the given driver table, indicating which interface the    //
// driver can support and how to create a driver instance.                   //
void Gtts_Register(DriverTable* table){                                    //                                  //
	table->AddDriver(DRIVERNAME, Gtts_Init);
}                                                                            //
// Extra stuff for building a shared object.                                 //
/* need the extern to avoid C++ name-mangling  */                            //
extern "C"{                                                                  //
	int player_driver_init(DriverTable* table){                              //
		puts("Gtts driver initiallized");
		Gtts_Register(table);
		puts("waiting for client startup...");                               //
		return(0);                                                           //
	}                                                                        //
}                                                                            //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
