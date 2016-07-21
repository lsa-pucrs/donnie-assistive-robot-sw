#ifndef protocol_message_types_h
#define protocol_message_types_h

#include <Arduino.h>
#include "config.h"
#include "protocol.h"

#define SYSTEM_DIO_CMD 27



uint8_t ping_flag;


unsigned char rx_data[BUFFER_SIZE], tx_data[BUFFER_SIZE];
unsigned int rx_data_count, tx_data_count;

typedef struct{
    uint8_t type;
    uint8_t data[BUFFER_SIZE];
    uint8_t size;
} Command;


Player *client = new Player();
Command cmd;


uint8_t readCommand();
void clearCommand();
void sendRangerMsg(unsigned char *data, unsigned int data_count);
void sendSystemDioMsg(uint8_t bitfield,  uint8_t count);
void sendBumperMsg(uint8_t *data,  uint8_t data_count);
void sendPowerMsg(float data);
void systemMsg(char *data);
void systemMsg(String data);
void sendPing();
void sendRequestConfig();
void sendEncoderMsg(int16_t ticksR, int16_t ticksL, int8_t speedR, int8_t speedL);

uint8_t receivedSystemDioCmd();
uint8_t receivedPingCmd();
uint8_t receivedConfigCmd();
uint8_t receivedVelCmd();
uint8_t receivedServoCmd();
uint8_t receivedBeep();

uint8_t readCommand(){
    int i;
    if(client->readData(rx_data,&rx_data_count)){
        cmd.type = rx_data[0];
        for(i=0;i<(rx_data_count-1);i++){
           cmd.data[i]= rx_data[i+1];
        }
        cmd.size=rx_data_count;
        return cmd.type;
    }
    return 0;
}

void clearCommand(){
    cmd.type = 0;
    *cmd.data = NULL;
    cmd.size = 0;
}

void sendRangerMsg(unsigned char *data, unsigned int data_count){
	int i;

	tx_data[0]=RANGERPACK;
	tx_data[1]=data_count; //quantity of sensors in this package

	for(i=0;i<data_count;i++){
		tx_data[i+2]=data[i];
	}

    tx_data_count = data_count+2; //in bytes, RANGERPACK(1 byte) + qntOfRangers(1 byte) + data_count(n bytes)

    //uint8_t v[] = {RANGERPACK,7,0x42,0x13,0x12,0x10,0xF0,0x45,0xF4};
    //memcpy(tx_data,v,9);
    //tx_data_count = 9;

    client->writeData(tx_data,tx_data_count);
}

void sendSystemDioMsg(uint8_t bitfield,  uint8_t count){
    tx_data[0]=DIOPACK;
    tx_data[1]=count; //qnt of valid bits
    tx_data[2]=bitfield; //bitfield
    

    tx_data_count = 3; //in bytes, DIOPACK(1 byte) + qntOfDios(1 byte) + bitfield(1 byte)

    client->writeData(tx_data,tx_data_count);
}

void sendBumperMsg(uint8_t *data,  uint8_t data_count){
    int i;

    tx_data[0]=BUMPERPACK;
    tx_data[1]=data_count; //qnt of valid bumpers

    for(i=0;i<data_count;i++){
        tx_data[i+2]=data[i];
    }

    tx_data_count = data_count+2; //in bytes, BUMPERPACK(1 byte) + qntOfBumpers(1 byte) + data_count(n bytes)

    client->writeData(tx_data,tx_data_count);
}

void sendPowerMsg(float data){
    tx_data[0]=POWERPACK;

    //convert Float to int16_t, with 3 decimal points
    int16_t aux= int16_t((data)*1000);
    tx_data[1]=aux & 0x00ff; //data;
    tx_data[2]=(aux >> 8) & 0x00ff; //data;
    ////

    tx_data_count = 1+1+1;
    client->writeData(tx_data,tx_data_count);
}


void systemMsg(char *data){
    int i;
    tx_data[0]=SYSTEMMESSAGEPACK;

    for (i = 0; data[i]!=0; ++i){
        tx_data[i+1]=data[i];
    }

    tx_data_count = 1 + i; //in bytes, SYSTEMMESSAGEPACK(1 byte) + message(n bytes)

    client->writeData(tx_data,tx_data_count);
}

void systemMsg(String data){
    int str_len = data.length() + 1; //+1 for \0 in the string array
    char char_array[str_len];
    data.toCharArray(char_array, str_len);

    systemMsg(char_array);
}

void sendPing(){
    tx_data[0]=PINGPACK;
    tx_data[1]=0x42; //some data
    tx_data_count = 2;
    client->writeData(tx_data,tx_data_count);
}

void sendRequestConfig(){
    int i;
    tx_data[0]=REQUESTCONFIGPACK;
    tx_data[1]=43; //some data

    tx_data_count = 2;

    client->writeData(tx_data,tx_data_count);
}

void sendEncoderMsg(int16_t ticksR, int16_t ticksL, int16_t speedR, int16_t speedL){
    int i;

    tx_data[0]=ENCODERPACK;
    tx_data[1]= (ticksR >> 8) & 0x00ff; //high byte
    tx_data[2]= ticksR & 0x00ff; //low byte
    tx_data[3]= (ticksL >> 8) & 0x00ff; //high byte
    tx_data[4]= ticksL & 0x00ff; //low byte

    
    tx_data[5]= (speedR >> 8) & 0x00ff; //high byte
    tx_data[6]= speedR & 0x00ff; //low byte
    tx_data[7]= (speedL >> 8) & 0x00ff; //high byte
    tx_data[8]= speedL & 0x00ff; //low byte
    
    /*
    tx_data[5]=(int8_t)speedR;
    tx_data[6]=(int8_t)speedL;*/


    tx_data_count = 5+4;

    client->writeData(tx_data,tx_data_count);  
}


uint8_t receivedSystemDioCmd(){
    if(cmd.type==DIOPACK) return 1;
    else return 0;
}
uint8_t receivedPingCmd(){
    if(cmd.type==PINGPACK) return 1;
    else return 0;
}
uint8_t receivedConfigCmd(){
    if(cmd.type==CONFIGPACK) return 1;
    else return 0;
}
uint8_t receivedVelCmd(){
    if(cmd.type==MOTORPACK) return 1;
    else return 0;
}
uint8_t receivedServoCmd(){
    if(cmd.type==SERVOPACK) return 1;
    else return 0;
}
uint8_t receivedBeep(){
    if(cmd.type==BEEPPACK) return 1;
    else return 0;
}

#endif