#include <libplayerc++/playerc++.h>
#include <iostream>

//////////////////
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <sys/types.h>
#define BAUDRATE B9600

//

void signal_handler_IO (int status);


// TO DO. change the serial port by parameter
// see port = cf->ReadString (section, "port", "/dev/ttyACM0");
// in driver/donnie/donnie.cc 
char devicename[80] = "/dev/ttyACM0", ch;
int status;
//

//////////////////
using namespace PlayerCc;
using namespace std;

//default arguments
// TO DO. change them by parameter
string host = "localhost"; 
int port = 6665;

int main(int argc, char** argv){

	PlayerClient robot(host,port);
	RangerProxy myranger(&robot,0);
//////////////////////////////
int fd, res, i;
struct termios newtio;
struct sigaction saio;
char buf[255];
char P[] = {'Z','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P'};
//
//open the device in non-blocking way (read will return immediately)
fd = open(devicename, O_RDWR | O_NOCTTY | O_NONBLOCK);
if (fd < 0)
{
perror(devicename);
exit(1);
}
//
//install the serial handler before making the device asynchronous
saio.sa_handler = NULL;//signal_handler_IO;
sigemptyset(&saio.sa_mask);   //saio.sa_mask = 0;
saio.sa_flags = 0;
saio.sa_restorer = NULL;
sigaction(SIGIO,&saio,NULL);
//
// allow the process to receive SIGIO
fcntl(fd, F_SETOWN, getpid());
//
// make the file descriptor asynchronous
fcntl(fd, F_SETFL, FASYNC);
//
// set new port settings for canonical input processing
newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
newtio.c_iflag = IGNPAR;
newtio.c_oflag = 0;
newtio.c_lflag = 0;
newtio.c_cc[VMIN]=1;
newtio.c_cc[VTIME]=0;
tcflush(fd, TCIFLUSH);
tcsetattr(fd,TCSANOW,&newtio);
//
// loop while waiting for input. normally we would do something useful here


/////////////////////////
	
	
	for(;;){
		write(fd,"Z",1);
		for(int i = 0; i <7; i++)
			write(fd,"A",1);
		for(int i = 0; i <7; i++)
			if( P[i+8]== 'A') write(fd,"A",1); else if (P[i+8] == 'B') write(fd,"B",1); else write(fd,"P",1);
		for(int i = 0; i <7; i++)
			write(fd,"A",1);
		for(int i = 0; i <7; i++)
			if( P[i+8]== 'A') write(fd,"A",1); else if (P[i+8] == 'B') write(fd,"B",1); else write(fd,"P",1);
		for(int i = 0; i <7; i++)
			write(fd,"A",1);
	
		cout << "Rangers:";
		for(int i=0;i<myranger.GetRangeCount();i++){  //print all lasers receiveds
			if((int(myranger[i]/10))==0) cout << "  " << myranger[i] << " ";  //unidade
			else if((int(myranger[i]/100))==0) cout << " " << myranger[i] << " ";  //dezena
			else cout << myranger[i] << " ";  //centena	
			if(myranger[i] < 1.) P[i+8] = 'A'; else if(myranger[i] < 1.5)	P[i+8] = 'B'; else P[i+8] = 'P';
			if(myranger[i] < 1.) P[i+22] = 'A'; else if(myranger[i] < 1.5)	P[i+22] = 'B'; else P[i+22] = 'P';
		}
			
		cout << "Qnt:" << myranger.GetRangeCount() << endl;
		robot.Read(); //update proxies  

		
	}
	close(fd);
	return 0;
}
