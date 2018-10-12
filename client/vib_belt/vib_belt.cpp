
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
    //cout << "done setting P array\n";

    // set device name
    if (device != NULL)
        strcpy(this->devicename, device);
    else
        strcpy(this->devicename, "/dev/ttyACM0");

	//cout << "done setting devicename\n";

    // set port and hostname
    if (port != 0) {
		cout << "[vib_belt] setting port to " << port << endl;
        this->port = port;
    }
    else {
		cout << "[vib_belt] setting port to 6665" << endl;
        this->port = 6665;
	}

    if (hostname != NULL) {
		cout << "[vib_belt] setting hostname to " << hostname << endl;
        this->host = string(hostname);
	}
    else {
		cout << "[vib_belt] setting hostname to \"localhost\"" << endl;
        this->host = string("localhost");
	}

	//cout << "done setting port & host" << endl;

	cout << "[vib_belt] " << this->host << " "  << this->port << " "  << this->devicename << endl;

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

	// TODO change the default value to 0 when serial is ok
	status= 1;
	robot->StartThread();
}

bool vib_belt :: connect() {
	cout << "[vib_belt] connecting" << endl;	
    this->fd = open(devicename, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (this->fd < 0){
		cout << "[vib_belt] error to open the serial port" << endl;
        return false;
	}
        

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

	status = 1;
    return true;
}


bool vib_belt :: loop() {
	
	if (status==0){
		cout << "[vib_belt] serial port not connected." << endl;
		return false;
	}
		
    // all writing operations are checked to see if they are working properly, in the case in which they aren't, the function will return false.
    //cout << "writing serial port ... " << endl;
    if (write(this->fd, (void*)this->P, 36) == 0){
		cout << "[vib_belt] error sending" << endl;
		// TODO uncomment this when serial port is actually sending data
		//return false;
	}


    for(int i = 0; i < this->myranger->GetRangeCount(); i++) {  //print all lasers receiveds
    
        // top line of vib motors . A is for critical distance (intense vib) and B is for mild vib. P stops vibrations
        if (this->myranger->GetRange(i) < INTENSE_VIBRATION_DISTANCE)
            this->P[i+8] = 'A';
        else if(this->myranger->GetRange(i) < MILD_VIBRATION_DISTANCE)
            this->P[i+8] = 'B';
        else
            this->P[i+8] = 'P';

		// bottom line of vib motors
        if (this->myranger->GetRange(i) < INTENSE_VIBRATION_DISTANCE)
            this->P[i+22] = 'A';
        else if (this->myranger->GetRange(i) < MILD_VIBRATION_DISTANCE)
            this->P[i+22] = 'B';
        else
            this->P[i+22] = 'P';
    }

	// TODO comment this line when it works 
	print_serial();
    return true;
}

void vib_belt :: exit() {
    close(this->fd);
}

void vib_belt :: print_serial() {
	cout << "[vib_belt] printing the serial port content: " << this->P << endl;
}


