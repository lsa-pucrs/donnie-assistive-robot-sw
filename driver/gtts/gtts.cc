/*
 * Desc: A robot Google Text-to-Speech handle
 * Author: Guilherme Marques
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

// TODO:  colocar um parametro p definir o idioma. deixar pt-br como default
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
#include <netinet/in.h> //TODO perguntar para henry oq isso faz
//#include <assert.h>
#include <fstream>      // std::ofstream
#include <ctype.h> // for toupper/tolower

using namespace std;

#define DRIVERNAME "gtts"

// Message levels
#define MESSAGE_ERROR	0
#define MESSAGE_INFO	1
#define MESSAGE_DEBUG	2

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
		PLAYER_ERROR("[Gtts] Could not read sound interface");
		SetError (-1);
		return;
	}
	//requires assosiation with interfaces
	if (cf->ReadDeviceAddr(&(this->speech_addr), section, "provides", PLAYER_SPEECH_CODE, -1, NULL)){
		PLAYER_ERROR("[Gtts] Could not read speech");
		SetError(-1);
		return;
	}
	if (AddInterface(this->speech_addr)){
		PLAYER_ERROR("[Gtts] Could not add speech interface");
		SetError(-1);
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device (ONLY WHEN A CLIENT CONECTS TO THE DRIVER).  Return 0 if things go well, and -1 otherwise.
int Gtts::MainSetup(){   
	PLAYER_MSG0(MESSAGE_INFO, "[Gtts] client has been connected");

	//assossiate the device with an interface
	//Gtts this driver with Sound driver like a client does
	this->sound_dev = deviceTable->GetDevice(this->sound_addr);
	if (!(this->sound_dev)) return -1;
	if (this->sound_dev->Subscribe(this->InQueue)){
		this->sound_dev = NULL;
		return -1;
	}

	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//Shutdown the device (occurs in each client shutdown)
void Gtts::MainQuit(){
	PLAYER_MSG0(MESSAGE_INFO, "[Gtts] client has been disconnected...");
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

	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_SPEECH_CMD_SAY, speech_addr)){
		ProcessSpeechCmd(hdr, *reinterpret_cast<player_speech_cmd_t*>(data));
		return 0;
	}

	return(-1);
}

void Gtts::ProcessSpeechCmd(player_msghdr_t* hdr, player_speech_cmd_t &data){

	char *palavra = data.string;
	#ifndef NDEBUG
	  PLAYER_MSG1(MESSAGE_INFO,"[Gtts] Receiving phrase [%s] to be transformed to speech",palavra);
	#endif
	//treat the white spaces ' '->%
	int i = 0;
	for(i = 0; i < data.string_count; i++)
	{
		if(*(palavra+i) == ' ')
		{
			*(palavra+i) = '+';
		}
	}

	// Determine language from DONNIE_LANG environment variable
	string lang = getenv("DONNIE_LANG");
	if (lang == "en_US") lang = "EN_us";
	if (lang == "pt_BR") lang = "PT_br";
	string langurl = "&tl=" + lang;

	char url[1000];
	//FULL BASE URL strcpy(url,"http://translate.google.com/translate_tts?ie=UTF-8&total=1&idx=0&textlen=32&client=tw-ob&tl=PT-br&q=");
	strcpy(url,"http://translate.google.com/translate_tts?");
	strcat(url,"ie=UTF-8");  // TODO. Assuming UTF8. It is required to reda the current system encoding
	//strcat(url,"ie=ISO-8859-1");
	strcat(url,"&total=1");
	strcat(url,"&idx=0");
	strcat(url,"&textlen=32");
	strcat(url,"&client=tw-ob");
	strcat(url,langurl.c_str());  // TODO. Assuming pt-br, this must change for internationalization
	strcat(url,"&q=");
	strcat(url, palavra);
	#ifndef NDEBUG
	  PLAYER_MSG1(MESSAGE_INFO,"[Gtts] URL for GoogleTTS [%s]",url);
	#endif	
	
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

	//Temporary filename generation
	char tmpName[1000];
	if(tmpnam(tmpName)){
		strcat(tmpName,".mp3"); //concat filetype

		char cmdcurl[1000];
	    strcpy(cmdcurl,"wget -q -U Mozilla -O ");
	    strcat(cmdcurl,tmpName); //filename
	    strcat(cmdcurl," \"");
	    strcat(cmdcurl,url);
	    strcat(cmdcurl,"\"");
		if (system(cmdcurl) == -1){
			PLAYER_ERROR("[Gtts] Cannot execute wget");
			return;
		}
		// publish the filename
	    Play(tmpName);
	}
	else{
		PLAYER_ERROR("[Gtts] Cannot generate the temp file");
	} 
}

void Gtts::Play(char *fileAddr){
	player_sound_cmd_t sfile;
	memset(&sfile,0,sizeof(sfile));

	strcpy(sfile.filename, fileAddr);
	#ifndef NDEBUG
	  PLAYER_MSG1(MESSAGE_INFO,"[Gtts] Sending filename [%s] to be played by sox",sfile.filename);
	#endif	

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
		PLAYER_MSG0(MESSAGE_INFO, "[Gtts] driver initiallized");
		Gtts_Register(table);
		PLAYER_MSG0(MESSAGE_INFO, "[Gtts] waiting for client startup...");
		return(0);                                                           //
	}                                                                        //
}                                                                            //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
