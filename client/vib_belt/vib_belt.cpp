
#include <libplayerc++/playerc++.h>
#include "vib_belt.h"
/////////////////////

using namespace PlayerCc;
using namespace std;

vib_belt::vib_belt(const char* hostname, int port) {
    // initialize P array
    this->P[0] = 'Z';
    for (int i = 1; i < 36; i++)
        this->P[i] = 'P';

    // set port and hostname
    if (port != 0) {
		cout << "[ranger2vib] setting port to " << port << endl;
        this->port = port;
    }
    else {
		cout << "[ranger2vib] setting port to 6665" << endl;
        this->port = 6665;
	}

    if (hostname != NULL) {
		cout << "[ranger2vib] setting hostname to " << hostname << endl;
        this->host = string(hostname);
	}
    else {
		cout << "[ranger2vib] setting hostname to \"localhost\"" << endl;
        this->host = string("localhost");
	}

	cout << "[ranger2vib] " << this->host << " "  << this->port << endl;

	try{
		this->robot = new PlayerClient(this->host, this->port);
		this->ranger = new RangerProxy(this->robot, 0);
		this->speech = new SpeechProxy(this->robot, 1);
	} catch (PlayerError e){
		#ifndef NDEBUG
			cerr << e << endl;
		#endif
		cerr << "Não foi possível conectar no robô " << endl;
		cerr << "Possivelmente o arquivo cfg está incorreto." << endl;
		exit(-1);
	}

	robot->StartThread();
}


bool vib_belt :: loop() {
	
    for(int i = 0; i < this->ranger->GetRangeCount(); i++) {  //print all lasers receiveds
    
        // top line of vib motors . A is for critical distance (intense vib) and B is for mild vib. P stops vibrations
        if (this->ranger->GetRange(i) < INTENSE_VIBRATION_DISTANCE)
            this->P[i+8] = 'A';
        else if(this->ranger->GetRange(i) < MILD_VIBRATION_DISTANCE)
            this->P[i+8] = 'B';
        else
            this->P[i+8] = 'P';

		// bottom line of vib motors
        if (this->ranger->GetRange(i) < INTENSE_VIBRATION_DISTANCE)
            this->P[i+22] = 'A';
        else if (this->ranger->GetRange(i) < MILD_VIBRATION_DISTANCE)
            this->P[i+22] = 'B';
        else
            this->P[i+22] = 'P';
    }

	// TODO comment this line when it works 
	cout << "[ranger2vib] printing the serial port content: " << this->P << endl;
	speech->Say(this->P);
    return true;
}




