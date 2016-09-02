/*
 * Desc: A robot alerts handle
 * Author: Guilherme Marques
 * Date:  August 2016
 */

/*
cfg example:

driver
(
	name "alerts"
	plugin "libalerts"
	requires ["bumper:0"] 
	provides ["speech:0"]
	#requires ["dio:0" "ranger:0" "base:::position2d:0" "bumper:0" "power:0"] 
	#requires ["note:0" "speech:0" "audioplayer:0"]
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
#include <curl/curl.h> //TODO perguntar para henry oq isso faz
//#include <netinet/in.h> //TODO perguntar para henry oq isso faz


#include "CFugueLib.h" //to play notes


#define DRIVERNAME "alerts"

CURL *curl;  //TODO perguntar para henry oq isso faz
FILE *file;


////////////////////////////////////////////////////////////////////////////////
// The class for the driver
// devices are used to communication between drivers
// interfaces used to communication between client and the driver
class Alerts : public ThreadedDriver{
	public:
		Alerts(ConfigFile* cf, int section);
		virtual int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data);

	private:
		player_devaddr_t m_speech_addr;  //speech interface
		player_devaddr_t bumper_addr;	//bumper interface
		Device *bumper_dev; //bumper device

		//cfugue
		CFugue::Player player;
		int nPortID=MIDI_MAPPER, nTimerRes = 20;

		virtual int MainSetup();
		virtual void MainQuit();
		virtual void Main();
		void ProcessBeepCmd(player_msghdr_t* hdr, player_beep_cmd_t &data);
		void Beep(uint16_t frequency,uint32_t duration);
		virtual void ProcessSpeechCmd(player_msghdr_t* hdr, player_speech_cmd_t &data);
		virtual void ProcessBumperData(player_bumper_data_t &data);
};

// Deal with assosiations comming by .cfg file
Alerts::Alerts(ConfigFile* cf, int section) : ThreadedDriver(cf, section){
	memset (&this->bumper_addr, 0, sizeof (player_devaddr_t));
	this->bumper_dev = NULL;

	//requires assosiations with devies
	if (cf->ReadDeviceAddr (&(this->bumper_addr),section,"requires",PLAYER_BUMPER_CODE, -1, NULL)){
		PLAYER_ERROR("Could not read bumper interface ");
		SetError (-1);
		return;
	}
	//requires assosiation with interfaces
	if (cf->ReadDeviceAddr(&(this->m_speech_addr), section, "provides", PLAYER_SPEECH_CODE, -1, NULL)){
		PLAYER_ERROR("Could not read SPEECH ");
		SetError(-1);
		return;
	}
	if (AddInterface(this->m_speech_addr)){
		PLAYER_ERROR("Could not add speech interface ");
		SetError(-1);
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device (ONLY WHEN A CLIENT CONECTS TO THE DRIVER).  Return 0 if things go well, and -1 otherwise.
int Alerts::MainSetup(){   
	puts("MainSetup driver initialising");

	//assossiate the device with an interface
	this->bumper_dev = deviceTable->GetDevice(this->bumper_addr);
	if (!(this->bumper_dev)) return -1;
	if (this->bumper_dev->Subscribe(this->InQueue)){
		this->bumper_dev = NULL;
		return -1;
	}


	//cfugue
	unsigned int nOutPortCount = CFugue::GetMidiOutPortCount();
	if(nOutPortCount <= 0)
		exit(fprintf(stderr, "No MIDI Output Ports found !!"));
	if(nOutPortCount > 1){
		_tprintf(_T("\n---------  The MIDI Out Port indices  --------\n\n"));
		/////////////////////////////////////////
		/// List the MIDI Ports
		for(unsigned int i=0; i < nOutPortCount; ++i)
        {
            std::string portName = CFugue::GetMidiOutPortName(i);
			std::wcout << "\t" << i << "\t: " << portName.c_str() << "\n";
        }
		//////////////////////////////////////////
		/// Chose a Midi output port
		_tprintf(_T("\nPlaying by default port 2"));
		//_tscanf(_T("%d"), &nPortID);
		nPortID = 1; //default port
	}
	else{
        std::string portName = CFugue::GetMidiOutPortName(0);
		std::wcout << "\nUsing the MIDI output port: " << portName.c_str();
	}
	_tprintf(_T("\n\n -------- C++ API Tests ------------\n"));

	nPortID = 1; //default port
    CFugue::Player player(nPortID, nTimerRes); // Create the Player Object
    usleep(10000);


	puts("MainSetup driver ready");
	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//Shutdown the device (occurs in each client shutdown)
void Alerts::MainQuit(){
	PLAYER_MSG0(0, "Alerts client has been shutdown...\n");
	PLAYER_MSG0(0, "Shutting Alerts driver down...");
	PLAYER_MSG0(0, "Done.\n");
	//return 0; //close server
}

////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void Alerts::Main(){
	for(;;){
		pthread_testcancel();
		this->ProcessMessages();
		usleep(1000);

	}
	return;
}


//deal with messages comming from the clients
int Alerts::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data){
	//PLAYER_WARN("msg Received Driver Alerts");
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_BUMPER_DATA_STATE, bumper_addr)){
		ProcessBumperData(*reinterpret_cast<player_bumper_data_t *>(data));
		//PLAYER_WARN("bumper data received");
		return(0);
	}
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_SPEECH_CMD_SAY, m_speech_addr)){
		ProcessSpeechCmd(hdr, *reinterpret_cast<player_speech_cmd_t*>(data));
		return 0;
	}

	return(-1);
}

void Alerts::ProcessSpeechCmd(player_msghdr_t* hdr, player_speech_cmd_t &data){
	std::cout << std::endl << "Texto:";
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
	strcpy(url,"translate.google.com/translate_tts?ie=UTF-8&total=1&idx=0&textlen=32&client=tw-ob&tl=pt-br&q=");
	strcat(url, palavra);
	//printf("%s\n", url);
	
	//Download do arquivo com curl
    file = fopen("download.mp3", "w");
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(file);

    //Inicio do arquivo
    //printf("Inicio\n");
	//DWORD streamHandle; // armazena o Stream de audio

	////Inicializa o Output de som, -1 para usar o device de som padrão do SO
	//if(!BASS_Init(-1, 44100, 0, 0, NULL))
	//{
	//	printf("Erro Inicialização\n");
	//}
	//
	//
	//// Carrega o arquivo de som
	//streamHandle = BASS_StreamCreateFile(FALSE, "download.mp3", 0, 0, BASS_SAMPLE_MONO);
	////int pos = BASS_ChannelGetLength(streamHandle,BASS_POS_BYTE);
	////Inicializa a Musica
	//if(BASS_ChannelPlay(streamHandle, FALSE))
	//{
	//	//printf("Play\n");
	//}
	//
	////Espera até o fim da musica
	//while (BASS_ChannelIsActive(streamHandle)) {
	//	Sleep(50);
	//}
	////Libera a Stream alocada
	//BASS_Free();
	////printf("Fim\n");
}

void Alerts::ProcessBumperData(player_bumper_data_t &data){
	uint8_t i;
	uint8_t isAnyBumped = 0;

	for(i=0;i<data.bumpers_count;i++){
		if(data.bumpers[i]) isAnyBumped = 1;
	}

	if(isAnyBumped){
		//system("beep");
		player.Play("Eq Gq Dh");
		puts("EEEEEEEEPAAAAAA");
		std::cout << "Bumpers(" << data.bumpers_count << ") ";
		for(i=0;i<data.bumpers_count;i++) 
			std::cout << (data.bumpers[i] ? "1 " : "0 ");
		std::cout << std::endl;
	}

}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// A factory creation function, declared outside of the class so that it     //
// can be invoked without any object context (alternatively, you can         //
// declare it static in the class).  In this function, we create and return  //
// (as a generic Driver*) a pointer to a new instance of this driver.        //
Driver* Alerts_Init(ConfigFile* cf, int section){                            //
	// Create and return a new instance of this driver                       //
	return ((Driver*) (new Alerts(cf, section)));
}                                                                            //
// A driver registration function, again declared outside of the class so    //
// that it can be invoked without object context.  In this function, we add  //
// the driver into the given driver table, indicating which interface the    //
// driver can support and how to create a driver instance.                   //
void Alerts_Register(DriverTable* table){                                    //                                  //
	table->AddDriver(DRIVERNAME, Alerts_Init);
}                                                                            //
// Extra stuff for building a shared object.                                 //
/* need the extern to avoid C++ name-mangling  */                            //
extern "C"{                                                                  //
	int player_driver_init(DriverTable* table){                              //
		puts("Alerts driver initiallized");
		Alerts_Register(table);
		puts("waiting for client startup...");                               //
		return(0);                                                           //
	}                                                                        //
}                                                                            //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////