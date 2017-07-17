#include <iostream>
#include <cstdlib>  // getenv
#include <string.h> // strcat
#include <libplayerc++/playerc++.h>
 
int main(int argc, char *argv[]){
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
	
	PlayerClient   robot(host.c_str(),port);
	PlaySoundProxy     sound(&robot, 0);

	if(argc>1)
		sound.play(argv[1]);
	else {
		if(const char* env_p = std::getenv("DONNIE_PATH")){
			std::cout << "DONNIE_PATH is: " << env_p << '\n';
                        char buff[80];
                        strcpy(buff, env_p);
                        strcat(buff,"/resources/sounds/step.wav");
			sound.play(buff);
		}else{
			std::cout << "variable DONNIE_PATH not defined. run 'export DONNIE_PATH=<path to donnie installation>'\n";
			return 1;
		}
	}

	return 0;
}
