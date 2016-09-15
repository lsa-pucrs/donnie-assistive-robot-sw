//Comments sources: 
//http://linux.die.net/man/3/cfsetispeed
//https://en.wikibooks.org/wiki/Serial_Programming/termios

#include     <stdio.h>      
#include     <string.h>
#include     <stdlib.h>     
#include     <unistd.h> 
#include     <sys/time.h>  //for select(), FD_SET(), FD_CLR(), FD_ISSET(), FD_ZERO() 
#include     <sys/types.h> //for select(), FD_SET(), FD_CLR(), FD_ISSET(), FD_ZERO() 
#include     <sys/stat.h>  //for select(), FD_SET(), FD_CLR(), FD_ISSET(), FD_ZERO() 
#include     <fcntl.h>     //for O_RDWR, O_NOCTTY, O_NDELAY      
#include     <termios.h>    
#include     <errno.h>   

#ifndef FALSE
    #define FALSE 0
#endif
#ifndef TRUE
    #define TRUE 1
#endif

//PROTOCOL DEFINES
#define DATAMAX 50
#define SYNC0   0xAA
#define SYNC1   0xBB
#define MOTORPACK  0x28
#define DIOPACK  0x15
#define RANGERPACK  0x68
#define BUMPERPACK  0x42
#define POWERPACK  0x66
#define SYSTEMMESSAGEPACK  0x73
#define CONFIGPACK  0xC0
#define REQUESTCONFIGPACK 0xC7
#define PINGPACK 0x41
#define ENCODERPACK  0x81
#define SERVOPACK  0x30
#define BEEPPACK 0x17


void sendPing();

class Serial{

    public:
        //Serial();
        Serial(const char *_port);
        ~Serial();
        void closePort();
        int readData(unsigned char *rx_data, unsigned int *rx_data_count);
        int writeData(unsigned char *tx_data, unsigned int tx_data_count);
        int writeData(unsigned char msgType, unsigned char *tx_data, unsigned int tx_data_count);
        int getState();
    private:
        void setPort(const char *_port);
        void setSpeed(void);
        void setParity();
        int openSerial();
        unsigned char checksum(const unsigned char *msg, unsigned char size);
        void toPrint();
        
        char serial_port[50];
        int fd; //The returned file handle for the device
        int state;
        unsigned char read_buff[256];
        unsigned char read_cs[1];
        int received_bytes;
        ssize_t expected_packet_size;
};


/*
Serial::Serial(){
    setPort((char*)"/dev/ttyACM0");
    openSerial();
}*/

Serial::Serial(const char *_port){
    setPort(_port);
    openSerial();
}

Serial::~Serial(){
    close(this->fd);
}

void Serial::closePort(){
    if(!close(this->fd))
        printf("Device disconected from %s\n",this->serial_port);
}

void Serial::setPort(const char *_port){
    strcpy(this->serial_port,_port);
}

int Serial::getState(){
    return this->state;
}

void Serial::setSpeed() {
    struct  termios Opt;
    tcgetattr(this->fd, &Opt);  //For reading the current attributes.
    cfsetispeed(&Opt,B115200);
    cfsetospeed(&Opt,B115200);  
    //speed must be one of these constants: B0, B50, B75, B110,B134,B150,B200,B300,B600,B1200,B1800,B2400,B4800,B9600,B19200,B38400,B57600,B115200, B230400
    tcsetattr(fd,TCSANOW,&Opt); //For setting serial interface attributes. 
    //TCSANOW -> the configuration is changed immediately.
}

void Serial::setParity() {
    struct termios options;
    tcgetattr(this->fd, &options); //For reading the current attributes.
    /*
    options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  //Input
    //ICANON -> Enables canonical input mode
    //ECHO -> Echo input characters. 
    //ECHOE -> If ICANON is also set, the ERASE character erases the preceding input character, and WERASE erases the preceding word. 
    //ISIG -> When any of the characters INTR, QUIT, SUSP, or DSUSP are received, generate the corresponding signal. 
    options.c_oflag  &= ~OPOST;   //Output
    //OPOST -> Enable implementation-defined output processing. */

    //Source http://stackoverflow.com/questions/8070632/missing-flow-control-data-0x13-from-reading-device-data-stream
    //this configs settings to see ALL characters and ignore nothing.
    //this fix the checksum error bug by control characters being sent through serial port 
    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    options.c_cflag &= ~(CSIZE | PARENB);
    options.c_cflag |= CS8;

    tcsetattr(fd,TCSANOW,&options); //For setting serial interface attributes. 
    //TCSANOW -> the configuration is changed immediately.
}

int Serial::openSerial() {
    this->fd = open(this->serial_port, O_RDWR);         //| O_NOCTTY | O_NDELAY);
    //O_RDWR -> Opens the port for reading and writing
    //O_NOCTTY -> The port never becomes the controlling terminal of the process.
    //O_NDELAY -> Use non-blocking I/O. On some systems this also means the RS232 DCD signal line is ignored.
    
    if(-1 == this->fd){ 
        printf("\nCan't Open Serial Port %s\n",this->serial_port);
        perror("Error");
        //std::cin.get();
        return -1;
    }
    else{
        printf("\nDevice connected at %s\n",this->serial_port);
        setSpeed();
        setParity();
        return 0;
    }
}

int Serial::readData(unsigned char *rx_data, unsigned int *rx_data_count){ //return 1 if there is valid data ro read
    fd_set rfds;
    struct timeval tv; //perguntar amory -> tem alguma diferença usar isso ou usar o sleep()?
  
    ssize_t length;

    int ready_descriptors; 

    unsigned char aux[1]; // temp data which is not buffered //gui - para que isso?

    ///Initialize the file descriptor set.
    FD_ZERO(&rfds); //Initializes the file descriptor set fdset to have zero bits for all file descriptors. 
    FD_SET(this->fd, &rfds); //Sets the bit for the file descriptor fd in the file descriptor set fdset. 
    
    //Initialize the timeout data structure.
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    //Wait for 5 seconds, then go

    ready_descriptors = select(this->fd + 1, &rfds, NULL, NULL, &tv); //returns the number of ready descriptors that are contained in the bit masks. If the time limit expires, select returns zero and sets errno to EINTR.
    
    //choose the target from set

    if(ready_descriptors > 0) {
        if (FD_ISSET(this->fd, &rfds)) { //FD_ISSET returns a non-zero value if the bit for the file descriptor fd is set in the file descriptor set pointed to by fdset, and 0 otherwise.
            switch(state){
                case 0: // expect the first header byte. does not buffer the byte
                    length = read(this->fd, aux, 1);
                    if ((int)length == 0){
                        // nothing received. do nothing
                        printf("\nnothing received\n"); //debug - deletar essa linha após
                        break;
                    }
                    if ((int)length != 1){
                        printf("\nserial error on state 0, SYNC0\n");
                        this->state = 0;
                    }

                    if (((int)length == 1) && (aux[0] == SYNC0)){
                        this->state = 1;
                        //printf("\nFOUND S0"); //debug - deletar essa linha após
                    }
                break;
                
                case 1: // expect the second header byte. does not buffer the byte
                    length = read(this->fd, aux, 1);
                    if ((int)length == 0){
                        // nothing received. do nothing
                        break;
                    }
                    if ((int)length != 1){
                        printf("\nserial error on state 1, SYNC1\n");
                        this->state = 0;
                    }
                    if ((int)(length == 1) && (aux[0] == SYNC1))
                        this->state = 2;
                        //printf("\nFOUND S1"); //debug - deletar essa linha após
                break;
                
                case 2: // get the expected packet size
                    length = read(this->fd, aux, 1);

                    if ((int)length == 0){
                        // nothing received. do nothing
                        break;
                    }
                    if ((int)length != 1){
                        printf("\nserial error on state 2, packet size\n");
                        this->state = 0;
                    }
                    if ((int)length == 1){
                        this->received_bytes = 0;
                        this->expected_packet_size = aux[0];
                        // check here the maximal size if relevant 
                        if(this->expected_packet_size <= DATAMAX)this->state = 3;
                        else{
                            this->state = 0;
                            break;
                        }
                        //printf("\nFOUND SZ"); //debug - deletar essa linha após
                    }
                break;

                // get the packet payload
                case 3:
                    length = read(this->fd, &this->read_buff[this->received_bytes], 1);

                    if ((int)length == 0){
                        // nothing received. do nothing
                        break;
                    }
                    if ((int)length < 0){
                        printf("\nserial error on state 3, payload\n");
                        this->state = 0;
                    }
                    else{ 
                        // if (0 < length < 256)
                        this->received_bytes += length;
                        if (this->expected_packet_size == this->received_bytes){
                            //printf("payload reveived\n"); //data
                            this->state = 4;
                        }
                        else if(this->expected_packet_size < this->received_bytes){
                            printf("\nPayload overloaded\n");
                            this->state = 0;
                        }
                    }
                break;

                // read 1st checksum byte
                case 4:
                    length = read(this->fd, aux, 1);
                    if (length == 0){
                        // nothing received. do nothing
                        break;
                    }
                    if (length != 1){
                        printf("\nserial error on state 4, checksum\n");
                        this->state = 0;
                    }
                    if (length == 1){
                        unsigned char CS_expected = aux[0];
                        unsigned char CS_result = checksum(this->read_buff,this->expected_packet_size);
                        
                        /* debug printa buffer sem tratamentp
                        if(this->read_buff[0]==0x68){
                            for(int k = 0; k<500;k++){
                                length = read(this->fd, aux, 1);
                                if(aux[0]==0x68){
                                    printf("%.2X ",aux[0]);
                                    for(int k = 0; k<20;k++){
                                        length = read(this->fd, aux, 1);
                                        printf("%.2X ",aux[0]);
                                    }   
                                    printf("===============\n");
                                }
                            }
                        }*/

                        if(CS_expected==CS_result){
                            memcpy(rx_data,this->read_buff,this->expected_packet_size);
                            *rx_data_count=this->expected_packet_size;
                            this->state = 0;
                            return 1; // found a valid data
                        }
                        else {
                            printf("\nChecksum Error!\n");
                            //checksum debug
                            int i;
                            //printf("Data:");
                            printf("%.2X ",(int)expected_packet_size);
                            for(i=0;i<this->expected_packet_size;i++) {
                                printf("%.2X ",this->read_buff[i]); //mostra a informacao, em hexa, contida no vetor rx_data
                            }
                            printf("%.2X\n",CS_expected);
                            //printf(",SZ:%.2X",(int)expected_packet_size);
                            printf("CS_result:%.2X\n",CS_result);
                            //printf("\nCS_result:%.2X\n",CS_result);

                            this->state = 0;
                        }
                    }
                break;
                default:
                    printf("\ninvalid state\n");
                    this->state = 0;
                break;
            }; //end switch

        }
        else{
            printf("\nThe bit for the file descriptor fd is not seted by fdset\n");
        }
    }
    else{
        printf("\nNo data within 5 seconds.\n");
    }
    return 0;//there is - non valid data 
}

/*
unsigned char Serial::checksum(unsigned char *msg, unsigned char size){
  int i;
  unsigned char sum = 0;
  for(i=0;i<size;i++){
    sum = sum ^ msg[i];
  }
  return sum;
}*/

//CRC-8 - based on the CRC8 formulas by Dallas/Maxim
//code released under the therms of the GNU GPL 3.0 license
//http://www.leonardomiliani.com/en/2013/un-semplice-crc8-per-arduino/
unsigned char Serial::checksum(const unsigned char *msg, unsigned char size) {
  unsigned char crc = 0x00;
  while (size--) {
    unsigned char extract = *msg++;
    for (unsigned char tempI = 8; tempI; tempI--){
      unsigned char sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}



int Serial::writeData(unsigned char msgType, unsigned char *tx_data, unsigned int tx_data_count){
    int i;
    for(i=tx_data_count;i>=0;i--){
        tx_data[i+1]=tx_data[i];
    }
    tx_data[0]=msgType;
    tx_data_count+=1;

    if(writeData(tx_data,tx_data_count)) return 1;
    else return 0;
}

int Serial::writeData(unsigned char *tx_data, unsigned int tx_data_count){
    unsigned int i, len;
    unsigned char temp[DATAMAX];
    temp[0]=SYNC0; //init byte0
    temp[1]=SYNC1; //init byte1
    temp[2]=tx_data_count; //message size byte
    for(i=0;i<tx_data_count+3;i++)
        temp[i+3]=tx_data[i];//passa mensagem para a variavel temp 
    temp[i]=checksum(tx_data,tx_data_count);
    i++;
    len=i;

    //printf("Write Dio temp[4]:%.2X\n",temp[4]);
    //printf("Write Dio tx_data[1]:%.2X\n",tx_data[1]);

    /*printf("debug write:");
    for(i=0;i<len;i++) {
        printf("%.2X",temp[i]);
         //show data in hexa
    }
    printf("\n");*/
    //std::cout << "Write Dio tx_data[1]:" << std::hex << tx_data[3] << std::endl; //decimal value

    if(!write(this->fd, temp, len)) printf("write error!\n");
    return 0;
}