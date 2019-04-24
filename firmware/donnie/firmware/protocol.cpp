#include <Arduino.h>
#include "protocol.h"
/*Define the serial used: Serial=PC communication and Serial3=Raspberry comm*/
#define SERIAL Serial3

/*TODO melhoria. Implementar um ping de mão-dupla para que não ocorra enchimento 
    do buffer Rx do Arduino caso haja uma desconexão momentânea
*/
void protocol_config(){
    SERIAL.begin(115200,SERIAL_8N1); // initialize the Serial communication
}

Player::Player(){
    //setPort((char*)"/dev/ttyACM0");
    //openPlayer();
    //toPrint();
}

Player::~Player(){
    //close(this->fd);
}

/*int Player::getState(){
    return this->state;
}*/

int Player::readData(unsigned char *rx_data, unsigned int *rx_data_count){ //return 1 if there is valid data ro read
    int length;
    unsigned char aux[1];


    if(SERIAL.available()>0){
        switch(this->state){
            case 0: // expect the first header byte. does not buffer the byte
                length = SERIAL.readBytes(aux, 1);
                if (length == 0){
                    // nothing received. do nothing
                    break;
                }
                if (length != 1){
                    printg("\nSerial error on state 0, SYNC0\n");
                    this->state = 0;
                }
                
                if ((length == 1) && (aux[0] == SYNC0)){
                    this->state = 1;
                    printg("\nFOUND S0"); //debug - deletar essa linha após
                }
                    
            break;
            
            case 1: // expect the second header byte. does not buffer the byte
                length = SERIAL.readBytes(aux, 1);
                if (length == 0){
                    // nothing received. do nothing
                    break;
                }
                if (length != 1){
                    printg("\nSerial error on state 1, SYNC1\n");
                    this->state = 0;
                }
                if ((length == 1) && (aux[0] == SYNC1))
                    this->state = 2;
                    printg("\nFOUND S1"); //debug - deletar essa linha após
            break;
            
            case 2: // get the expected packet size
                length = SERIAL.readBytes(aux, 1);

                if (length == 0){
                    // nothing received. do nothing
                    break;
                }
                if (length != 1){
                    printg("\nSerial error on state 2, packet size\n");
                    this->state = 0;
                }
                if (length == 1){
                    this->received_bytes = 0;
                    this->expected_packet_size = aux[0];
                    // check here the maximal size if relevant 
                    if(this->expected_packet_size <= DATAMAX)this->state = 3;
                    else{
                        this->state = 0;
                        break;
                    }
                    printg("\nFOUND SZ"); //debug - deletar essa linha após
                }
            break;


            // get the packet payload
            case 3:
                length = SERIAL.readBytes(&this->read_buff[this->received_bytes], 1);

                if (length == 0){
                    // nothing received. do nothing
                    break;
                }
                if (length < 0){
                    printg("\nSerial error on state 3, payload\n");
                    this->state = 0;
                }
                else{ 
                    // if (0 < length < 256)
                    this->received_bytes += length;
                    if (this->expected_packet_size == this->received_bytes){
                        printg("\nPayload reveived"); //data
                        this->state = 4;
                    }
                    else if(this->expected_packet_size < this->received_bytes){
                        printg("\nPayload overloaded\n");
                        this->state = 0;
                    }
                }
            break;

            // read 1st checksum byte
            case 4:
                length = SERIAL.readBytes(aux, 1);
                if (length == 0){
                    // nothing received. do nothing
                    break;
                }
                if (length != 1){
                    printg("\nSerial error on state 4, checksum\n");
                    this->state = 0;
                }
                if (length == 1){
                    unsigned char CS_expected = aux[0];
                    unsigned char CS_result = checksum(read_buff,this->expected_packet_size);
                    
                    if(CS_expected==CS_result){
                        memcpy(rx_data,this->read_buff,this->expected_packet_size);
                        *rx_data_count=this->expected_packet_size;
                        this->state = 0;
                        printg("\nChecksum success!");
                        return 1; // found a valid data
                    }
                    else {
                        printg("\nChecksum Error!");
                        this->state = 0;
                    }
                }
            break;
            
            
            default:
                printg("\ninvalid state\n");
                this->state = 0;
            break;
        }; //end switch
    }

    return 0;//there is - non valid data 
}

/*
uint8_t Player::checksum(uint8_t *msg, uint8_t size){
  uint8_t i;
  uint8_t sum = 0;
  for(i=0;i<size;i++){
    sum = sum ^ msg[i];
  }
  return sum;
}*/


//CRC-8 - based on the CRC8 formulas by Dallas/Maxim
//code released under the therms of the GNU GPL 3.0 license
//http://www.leonardomiliani.com/en/2013/un-semplice-crc8-per-arduino/
uint8_t Player::checksum(const uint8_t *msg, uint8_t size) {
  uint8_t crc = 0x00;
  while (size--) {
    uint8_t extract = *msg++;
    for (uint8_t tempI = 8; tempI; tempI--){
      uint8_t sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}




/*
Erratic 2bytes checksum - ainda nao testadi
int16_t Player::checksum2(uint8_t *msg, int16_t size) {
     uint16_t sum = 0;
     while (size > 1) {
         sum += (*(msg)<<8) | *(msg+1);
         size -= 2;
         msg += 2;
     }
     if (size > 0) sum = sum ^ (uint16_t)*msg;
     return sum;
}*/

/*
int Player::writeData(unsigned char msgType, unsigned char *tx_data, unsigned int tx_data_count){
    int i;
    for(i=tx_data_count;i>=0;i--){
        tx_data[i+1]=tx_data[i];
    }
    tx_data[0]=msgType;
    tx_data_count+=1;

    if(writeData(tx_data,tx_data_count)) return 1;
    else return 0;
}*/

int Player::writeData(uint8_t *tx_data, unsigned int tx_data_count){
    //noInterrupts(); //desativa as interrupcoes para evitar que alguma seja chamada durante a transmissao
    int i, len;
    uint8_t temp[DATAMAX];
    temp[0]=SYNC0; //init byte0
    temp[1]=SYNC1; //init byte1
    temp[2]=tx_data_count; //message size byte
    for(i=0;i<tx_data_count+3;i++) temp[i+3]=tx_data[i];//passa mensagem para a variavel temp 
    temp[i]=checksum(tx_data,tx_data_count);
    i++;
    len=i;    
    SERIAL.write(temp,len);
    SERIAL.flush(); // espera o fim da transmissao
    //interrupts(); //reativa a possibilidade de interrupcoes
    return 0;
}

int Player::printg(char *msg){ //generic string print
    int i;
    for(i=0;msg[i]!='\0';i++){
        SERIAL.write(msg[i]);
        if(i>=200)return 1; //maybe the msg printed is incorrect
    }
    return 0;
}
