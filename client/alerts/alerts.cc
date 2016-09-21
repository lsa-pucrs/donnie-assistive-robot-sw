#include <iostream>
#include <libplayerc++/playerc++.h>
#define RSCPATH "../resources/sounds/"

int main(int argc, char *argv[]){
	using namespace PlayerCc;
	//PlayerClient    robot("192.168.0.113",6665);
	PlayerClient    robot("localhost",6665);
	SoundProxy     sound(&robot, 0);
	if(argc>1)sound.play(argv[1]);
	else sound.play(RSCPATH"right.wav");

	return 0;
}
