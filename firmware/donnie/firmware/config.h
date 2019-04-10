/*aqui vao os a declaracao dos pinos e as definicoes estaticas do donnie tais como carga
maxima da bateria e carga minima e quantidade de sensores sendo usados etc


*/

/*
USAR IDE ARDUINO 1.2 OU SUPERIOR 

 http://forum.arduino.cc/index.php?topic=45742.0 - checksum
 LiquidCrystal Library - Serial Input
 The lcd circuit:
 VDD VCC
 VSS GND

 LCD R/W pin to ground
 10K potenciometer to LCD VO 
*/


#ifndef config_h
#define config_h

#include <Arduino.h>


//BATERRY PIN
#define PIN_BATTERY 0 //Analogic input

//SYSTEM DIO PINS
#define PIN_LED_BATTERY 52
#define PIN_LED_CONNECTION 50
#define PIN_VIBRATION_R 48
#define PIN_VIBRATION_L 38
#define PIN_BUZZER 36

//BUMPER PINS
#define PIN_BUMPER_NE 47
#define PIN_BUMPER_NW 49
#define PIN_BUMPER_SW 51
#define PIN_BUMPER_SE 53

//DANIEL MOTORS DRIVER PINS
//Motor A (RIGHT)
#define RPWMA 13
#define RAIN1 8
#define RAIN2 12
 //Motor B (LEFT)
#define LPWMB 11
#define LBIN1 9
#define LBIN2 10
 //Motors standby mode
#define STBY 7 //This pi9n must be driven high to take the driver out of standby mode.
 //encoder
#define pololuAL 2 //interrupt pin
#define pololuBL 34
#define pololuAR 3 //interrupt pin
#define pololuBR 33
//#define kP 3
//#define kI 0 // nao utilizado
//#define kD 0 // nao utilizado

/*

//MOTORS DRIVER PINS
//Motor A (LEFT)
#define PWMA 11 //Speed control 
#define AIN1 9 //Direction
#define AIN2 10 //Direction
//Motor B (RIGHT)
#define PWMB 13 //Speed control
#define BIN1 8 //Direction
#define BIN2 12 //Direction
//Motors standby mode
#define STBY 7 //This pi9n must be driven high to take the driver out of standby mode.
*/

 //SERVO PIN
#define PIN_SERVO_NECK 25

//ENCODER PINS
#define INTERRUPT_ENCODER_R_M1A 1 //1 = int1 pin3
#define INTERRUPT_ENCODER_L_M2A 0 //0 = int0 pin2
#define PIN_ENCODER_R_M1B 33
#define PIN_ENCODER_L_M2B 34

//BATERRY PARAMETERS
#define BATTERY_FULL 8.3  //for S2 4.15v each cell
#define BATTERY_LOW_BATTERY_ALERT 7.1 //7
#define BATTERY_EMPTY 6.6 //for S2 3.3v each cell
#define ANALOG_EQUIV_FULL 535 //the value receiveid in the arduino pin equivalent of full voltage (ex: 535 = 8.4v)
#define ANALOG_EQUIV_EMPTY 430

//UPDATE FREQUENCY PARAMETERS
#define UPDATE_DIO_FRQ 50  
#define UPDATE_RANGER_FRQ 10
#define UPDATE_BUMPER_FRQ 10
#define UPDATE_BATTERY_FRQ 100
#define UPDATE_ENCODER_FRQ 5
#define UPDATE_MOTORS_FRQ 50
#define UPDATE_SERVOS_FRQ 50
//#define UPDATE_PING_FRQ 50

//SEND FREQUENCY PARAMETERS 
#define SEND_DIO_FRQ 300
#define SEND_RANGER_FRQ 20
#define SEND_BUMPER_FRQ 20
#define SEND_BATTERY_FRQ 300
#define SEND_ENCODER_FRQ 10
#define SEND_SYSTEMMSG_FRQ 300
#define SEND_REQUESTCONFIG_FRQ 1000
#define SEND_PING_FRQ 2000

#endif