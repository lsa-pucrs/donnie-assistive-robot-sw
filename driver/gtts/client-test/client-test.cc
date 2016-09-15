#include <iostream>
#include <libplayerc++/playerc++.h>

int main(int argc, char *argv[]){
	using namespace PlayerCc;
	//PlayerClient    robot("192.168.0.113",6665);
	PlayerClient    robot("localhost",6665);
	SpeechProxy     sp(&robot, 0);

	sp.Say("O mundo inteiro é um palco\n");
    usleep(3000000);
    sp.Say("E todos os homens e mulheres são meros atores\n");
    usleep(3500000);
    sp.Say("Têm suas saídas e suas entradas\n");
    usleep(3500000);
    sp.Say("E um homem cumpre em seu tempo muitos papéis\n");
    usleep(3000000);
    sp.Say("Trecho da peça As You Like It, escrito por William Shakespeare\n");
    usleep(3000000);

	return 0;
}
