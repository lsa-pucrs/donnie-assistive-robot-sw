#ifndef protocol_h
#define protocol_h

#include <Arduino.h>

#define BUFFER_SIZE 200

#define FALSE 0
#define TRUE 1

//PROTOCOL DEFINES
#define DATAMAX 50
#define SYNC0 0xAA
#define SYNC1 0xBB
#define BUMPERPACK 0x42
#define RANGERPACK 0x68
#define LINEFOLLOWERPACK 0x93
#define DIOPACK 0x15
#define MOTORPACK 0x28
#define POWERPACK 0x66
#define SYSTEMMESSAGEPACK 0x73
#define CONFIGPACK 0xC0
#define REQUESTCONFIGPACK 0xC7
#define PINGPACK 0x41
#define ENCODERPACK 0x81
#define SERVOPACK 0x30
#define BEEPPACK 0x17

//amory - implementar contador de erro de checksum e mandar de tempos em tempos pro driver

void protocol_config();

class Player {
   public:
    Player();
    ~Player();
    int readData(unsigned char *rx_data, unsigned int *rx_data_count);
    int writeData(unsigned char *tx_data, unsigned int tx_data_count);
    int writeData(unsigned char msgType, unsigned char *tx_data, unsigned int tx_data_count);

   private:
    uint8_t checksum(const uint8_t *msg, uint8_t size);
    int printg(char *msg);  //generic string print

    char serial_port[50];
    int fd;  //The returned file handle for the device
    int state;
    unsigned char read_buff[256];
    int received_bytes;
    int expected_packet_size;
};

#endif
