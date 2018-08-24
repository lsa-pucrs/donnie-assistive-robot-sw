
#include <libplayerc++/playerc++.h>
#include "vib_belt.h"
/////////////////////

using namespace PlayerCc;
using namespace std;

vib_belt::vib_belt(const char* device, const char* hostname, int port) {
    // initialize P array
    this->P[0] = 'Z';
    for (int i = 1; i < 36; i++)
        this->P[i] = 'P';
    /////////////////////
    cout << "done setting P array\n";

    // set device name
    if (device != NULL)
        strcpy(this->devicename, device);
    else
        strcpy(this->devicename, "/dev/ttyACM0");

	cout << "done setting devicename\n";

    // set port and hostname
    if (port != 0) {
		cout << "setting port to " << port << endl;
        this->port = port;
    }
    else {
		cout << "setting port to 6665" << endl;
        this->port = 6665;
	}

    if (hostname != NULL) {
		cout << "setting hostname to " << hostname << endl;
        this->host = string(hostname);
	}
    else {
		cout << "setting hostname to \"localhost\"" << endl;
        this->host = string("localhost");
	}

	cout << "done setting port & host" << endl;

	cout << this->host << endl << this->port << endl;

	try{
		this->robot = new PlayerClient(this->host, this->port);
		this->myranger = new RangerProxy(this->robot, 0);
	} catch (PlayerError e){
		#ifndef NDEBUG
			cerr << e << endl;
		#endif
		cerr << "Não foi possível conectar no robô " << endl;
		cerr << "Possivelmente o arquivo cfg está incorreto." << endl;
		this->exit();
	}

	robot->StartThread();
    // set robot and rangers
    //this->robot(this->host, this->port);
    //this->myranger(&this->robot, 0);
}

bool vib_belt :: connect() {
    this->fd = open(devicename, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (this->fd < 0)
        return false;

    int res, i;
    //struct terminos newtio;
    struct termios newtio;
    //struct sigaction saio;
    struct sigaction saio;
    char buf[255];

    // Install the serial handler before making the device
    saio.sa_handler     = NULL; // signal_handler_IO;
    sigemptyset(&saio.sa_mask); // saio.sa_mask = 0;
    saio.sa_flags       = 0;
    saio.sa_restorer    = NULL;
    sigaction(SIGIO, &saio, NULL);
    // Allow the process to receive SIGIO
    fcntl(this->
        fd, F_SETOWN, getpid());
    // Make the fil descriptor asynchronous
    fcntl(this->fd, F_SETFL, FASYNC);
    // Set new port settings for canonical input processing
    newtio.c_cflag      = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag      = IGNPAR;
    newtio.c_oflag      = 0;
    newtio.c_lflag      = 0;
    newtio.c_cc[VMIN]   = 1;
    newtio.c_cc[VTIME]  = 0;

    tcflush(this->fd, TCIFLUSH);
    tcsetattr(this->fd,TCSANOW,&newtio);

    return true;
}

char* vib_belt::getP() {
	return this->P;
}

bool vib_belt :: loop() {
    // all writing operations are checked to see if they are working properly, in the case in which they aren't, the function will return false.
    cout << "writing ... " << endl;
    write(this->fd, (void*)this->getP(), 36);
	cout << "reading1 ... " << endl;
	//robot->Read(); //update proxies
	
    cout << "Rangers: " << this->myranger->GetRangeCount() << endl;
    cout << "reading2 ... " << endl;
    cout << "Rangers: " << this->myranger->GetRangeCount() << endl;
    cout << "reading3 ... " << endl;

    for(int i = 0; i < this->myranger->GetRangeCount(); i++) {  //print all lasers receiveds
		cout << "entrei no for do loop" << endl;
        if((int(this->myranger->GetRange(i) / 10)) == 0)
            //cout << "  " << this->myranger[i] << " ";  //unidade
            cout << "  " << this->myranger->GetRange(i) << " ";  //unidade
        else if ((int(this->myranger->GetRange(i) / 100)) == 0)
            cout << " " << this->myranger->GetRange(i) << " ";   //dezena
        else
            cout << this->myranger->GetRange(i) << " ";          //centena

    //	if(SensorValue < Distance of Max. Vibration value)...elseif(SensorValue < Distance of moderate Vibration)...
        if (this->myranger->GetRange(i) < 1.)
            this->P[i+8] = 'A';
        else if(this->myranger->GetRange(i) < 1.5)
            this->P[i+8] = 'B';
        else
            this->P[i+8] = 'P';

        if (this->myranger->GetRange(i) < 1.)
            this->P[i+22] = 'A';
        else if (this->myranger->GetRange(i) < 1.5)
            this->P[i+22] = 'B';
        else
            this->P[i+22] = 'P';
    }

    //cout << "Qnt:" << this->myranger->GetRangeCount() << endl;
    
	//cout << "outra coisa" << endl;
	cout << "entrei o loop" << endl;
    return true;
}

void vib_belt :: exit() {
    close(this->fd);
}
