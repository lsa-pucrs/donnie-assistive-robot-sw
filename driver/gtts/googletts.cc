/*
 * Desc: A simple example of beep proxy uses.
 * Author: Guilherme Marques
 * Date:  July 2016
 * CVS: $Id: GoogleTTS.cc 8000 2009-07-12 10:53:35Z gbiggs $
 */


/*
cfg example:

driver
(
  name "GoogleTTS"
  plugin "/usr/local/lib64/libGoogleTTS.so" #depende do endereço em que foi parar o .so durante o sudo make install
  provides ["6664:beep:0"]
)
*/

#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <libplayercore/playercore.h>
#include <iostream>
#include <sys/time.h>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "bass.h"
#define Sleep(x) usleep(x*1000)

//translate.google.com/translate_tts?ie=UTF-8&total=1&idx=0&textlen=32&client=tw-ob&q=Dia Lindo&tl=pt-br

CURL *curl;
FILE *file;

class GoogleTTS : public ThreadedDriver{
	public:
		GoogleTTS(ConfigFile* cf, int section);
		virtual int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data);

	private:
		player_devaddr_t m_speech_addr;  
		virtual void Main();
		virtual int MainSetup();
		virtual void MainQuit();
		virtual void ProcessSpeechCmd(player_msghdr_t* hdr, player_speech_cmd_t &data);

};

GoogleTTS::GoogleTTS(ConfigFile* cf, int section) : ThreadedDriver(cf, section){
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

int GoogleTTS::MainSetup(){   
	puts("MainSetup driver initialising");
	//reset variables
	puts("MainSetup driver ready");
	return(0);
}

void GoogleTTS::Main(){
	for(;;){
		pthread_testcancel();
		ProcessMessages();
		usleep(1000);
	}
	return;
}

void GoogleTTS::MainQuit(){
	PLAYER_MSG0(0, "Shutting speech driver down...");
	 //StopThread();
	 PLAYER_MSG0(0, "Driver speech has been shutdown\n");
}

int GoogleTTS::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data){
	 if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_SPEECH_CMD_SAY, m_speech_addr)){
			ProcessSpeechCmd(hdr, *reinterpret_cast<player_speech_cmd_t*>(data));
			

			return 0;
	 }
	return(-1);
}

void GoogleTTS::ProcessSpeechCmd(player_msghdr_t* hdr, player_speech_cmd_t &data){
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
	DWORD streamHandle; // armazena o Stream de audio

	//Inicializa o Output de som, -1 para usar o device de som padrão do SO
	if(!BASS_Init(-1, 44100, 0, 0, NULL))
	{
		printf("Erro Inicialização\n");
	}


	// Carrega o arquivo de som
	streamHandle = BASS_StreamCreateFile(FALSE, "download.mp3", 0, 0, BASS_SAMPLE_MONO);
	//int pos = BASS_ChannelGetLength(streamHandle,BASS_POS_BYTE);
	//Inicializa a Musica
	if(BASS_ChannelPlay(streamHandle, FALSE))
	{
		//printf("Play\n");
	}

	//Espera até o fim da musica
	while (BASS_ChannelIsActive(streamHandle)) {
		Sleep(50);
	}
	//Libera a Stream alocada
	BASS_Free();
	//printf("Fim\n");
}

///////////////////////////////////////////////////////////////////////////////////////////
Driver* GoogleTTS_Init(ConfigFile* cf, int section){ return ((Driver*) (new GoogleTTS(cf, section)));}
void GoogleTTS_Register(DriverTable* table){ table->AddDriver("googletts", GoogleTTS_Init);}
extern "C"{
	int player_driver_init(DriverTable* table)
	{
		puts("Arduino driver initializing");
		GoogleTTS_Register(table);
		puts("waiting for client start GoogleTTS...");
		return(0);
	}
}