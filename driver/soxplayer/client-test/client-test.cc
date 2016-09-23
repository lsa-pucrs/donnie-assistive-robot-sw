#include <iostream>
#include <cstdlib>  // getenv
#include <libplayerc++/playerc++.h>
 
int main(int argc, char *argv[]){
	using namespace PlayerCc;
	//PlayerClient    robot("192.168.0.113",6665);
	PlayerClient    robot("localhost",6665);
	SoundProxy     sound(&robot, 0);

	if(argc>1)
		sound.play(argv[1]);
	else {
		if(const char* env_p = std::getenv("DONNIE_PATH")){
			std::cout << "DONNIE_PATH is: " << env_p << '\n';
			sound.play(env_p && "/resouces/step.wav");
		}else{
			std::cout << "variable DONNIE_PATH not defined. run 'export DONNIE_PATH=<path to donnie installation>'\n"
			return 1;
		}
	}

	return 0;
}
