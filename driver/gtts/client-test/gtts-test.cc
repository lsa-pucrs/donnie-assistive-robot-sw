#include <iostream>
#include <libplayerc++/playerc++.h>

int main(int argc, char** argv)
{
	using namespace PlayerCc;
	
    //default arguments
    std::string host = "localhost"; 
    int port = 6665;
  
    //Arguments treatment
    for(int i=1;i<argc;i++){      
    	if(argv[i][0]=='-'&argv[i][1]=='h'){
    	  host = argv[i+1];  
    	  i=i+1;
    	}
    	else if(argv[i][0]=='-'&argv[i][1]=='p'){
    	  port = atoi(argv[i+1]);  //convert to int
    	  i=i+1;
    	}
    	else {
    	  std::cout << std::endl << "./main [options]" << std::endl;
    	  std::cout << "Where [options] can be:" << std::endl;
    	  std::cout << "  -h <ip>        : host ip where Player is running.Default: localhost" << std::endl;
    	  std::cout << "  -p <port>      : port where Player will listen. Default: 6665" << std::endl << std::endl;
    	  return -1;
    	}
    }
	
	PlayerClient    robot(host.c_str(),port);
	SpeechProxy     sp(&robot, 0);

	sp.Say("O mundo inteiro é um palco\n");
    sp.Say("E todos os homens e mulheres são meros atores\n");
    sp.Say("Têm suas saídas e suas entradas\n");
    sp.Say("E um homem cumpre em seu tempo muitos papéis\n");
    sp.Say("Trecho da peça As You Like It, escrito por William Shakespeare\n");
    usleep(15000000);

	return 0;
}
