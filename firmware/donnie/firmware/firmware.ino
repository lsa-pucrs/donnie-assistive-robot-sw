/*
PARA COMPILAR USAR IDE ARDUINO 1.2 OU SUPERIOR 
*/

#include <Arduino.h>
#include "config.h"
#include <Wire.h>
#include <string.h>
#include "alerts.h"
#include "messages.h"
#include "motors.h"
#include "Ranger.h"
#include "bumper.h"
#include <Servo.h>
#include "pitches.h"
#include "TimerThree.h"

Servo neckServo;



/*TODO perguntar para o amory se eh uma boa ideia implementar uma logica de bits de status
robot status:
	waiting config file
	connection lost
	critical baterry
*/

//uint32_t counter = 0;

//SENSORS DATA GLOBAL VARIABLES 
uint8_t systemDio_data = 0;

//timestamp vars 
unsigned long timestamp;
unsigned long encoderTimestamp; //time stamp

//Protocol times TODO Usar a biblioteca Arduino Scheduler para deixar o codigo mais limpo ao tratar dessas chamadas
unsigned long updateDioLastPrd, 
	updateRangerLastPrd, 
	updateBumperLastPrd, 
	updateBatteryLastPrd, 
	updateEncoderLastPrd, 
	updateMotorsLastPrd, 
	updateServosLastPrd, 
	updatePingLastPrd, 
	sendDioLastPrd, 
	sendRangerLastPrd, 
	sendBumperLastPrd, 
	sendBatteryLastPrd, 
	sendEncoderLastPrd, 
	sendSystemMsgLastPrd, 
	sendPingLastPrd,
	sendRequestLastPrd;

float BytesToFloat(uint8_t *data, uint8_t first){
	uint8_t aux[4];
	aux[0] = data[first];
	aux[1] = data[first+1];
	aux[2] = data[first+2];
	aux[3] = data[first+3];
	return BytesToFloat(aux);
}
float BytesToFloat(uint8_t *data){
	union {
	  float f;
	  uint8_t b[4]; //u.b[3] = b0;
	} u;

	u.b[3]=data[0];
	u.b[2]=data[1];
	u.b[1]=data[2];
	u.b[0]=data[3];
	return u.f;
}

uint16_t BytesToUint16_t(uint8_t *data, uint8_t first){
	union {
	  uint16_t v;
	  uint8_t b[2]; //u.b[1] = b0;
	} u;

	u.b[1] = data[first];
	u.b[0] = data[first+1];
	return u.v;
}

uint32_t BytesToUint32_t(uint8_t *data, uint8_t first){
	union {
	  uint32_t v;
	  uint8_t b[4]; //u.b[3] = b0;
	} u;

	u.b[3]=data[first];
	u.b[2]=data[first+1];
	u.b[1]=data[first+2];
	u.b[0]=data[first+3];
	return u.v;
}


void processCommand(){  
	if(receivedSystemDioCmd()){  //TODO trocar toda infraextrutura para SYSTEM_DIO_CMD //change state of the digital output
		digitalWrite(PIN_LED_BATTERY, cmd.data[0] & 1);             //-------x
		digitalWrite(PIN_LED_CONNECTION, (cmd.data[0] >> 1) & 1); 	//------x-
		digitalWrite(PIN_BUZZER, (cmd.data[0] >> 2) & 1);         	//-----x--
		digitalWrite(PIN_VIBRATION_L, (cmd.data[0] >> 3) & 1);   	//----x---
		digitalWrite(PIN_VIBRATION_R, (cmd.data[0] >> 4) & 1);    	//---x----
	}
	else if(receivedPingCmd()){
		lostConnectionFlag = 0;
		ping_flag = 0;
		//digitalWrite(PIN_LED_CONNECTION,HIGH);
		//systemMsg("Ping Arduino Received");
	}
	else if(receivedVelCmd()){
		//m.control_movement();
		motorsCommandProcess(cmd.data);
	}
	else if(receivedServoCmd()){
		//TODO --definir as configuracoes do servo--if(cmd.data[0]==SCAN//SETPOS//GOTO(POS,VEL))
		//setNeckServoPos(cmd.data);
		neckServo.write(cmd.data[1]);
	}
	else if(receivedConfigCmd()){
		m.setKP(BytesToFloat(cmd.data,0));
		m.setKI(BytesToFloat(cmd.data,4));
		m.setKD(BytesToFloat(cmd.data,8));
		m.setSetpoint(BytesToFloat(cmd.data,12));
		m.setOffsetR(cmd.data[16]);
		m.setOffsetL(cmd.data[17]);
	}
	else if(receivedBeep()){
		//TODO definir mensagem com o tipo do beep -> Melody/BeepWithDuration/BeepActiveOn/BeepActiveOff
		tone(PIN_BUZZER, BytesToUint16_t(cmd.data,0), BytesToUint32_t(cmd.data,2));
		systemMsg("\nBeep test Start\n");
		systemMsg(String(BytesToUint16_t(cmd.data,0)));
		systemMsg("\n");
		systemMsg(String(BytesToUint32_t(cmd.data,2)));
		systemMsg("\nBeep test End\n");
	}
	
	
	clearCommand();
}

/*void updateActuators(){
	//motorsCommandProcess(B10011001);
	if(millis() - updateMotorsLastPrd >= UPDATE_MOTORS_FRQ){
		updateMotorsLastPrd = millis();
		updateMotorsLastPrd = millis();
		motorsUpdate();   //update pid
	}
	/*if(millis() - updateServosLastPrd >= UPDATE_SERVOS_FRQ){
		updateServosLastPrd = millis();
		neckServo.write(pos);
	}
}*/

//Debug Timestamp
//unsigned int bumperTsFlag=0,rangerTsFlag=0;
//unsigned long bumperTs,rangerTs;
//#Debug Timestamp
void updateSensors(){
	if(millis() - updateDioLastPrd >= UPDATE_DIO_FRQ){
		updateDioLastPrd = millis();
		systemDio_data = 0; //0000 0000
		systemDio_data = systemDio_data | digitalRead(PIN_LED_BATTERY); //0000 000X
		systemDio_data = systemDio_data | (digitalRead(PIN_LED_CONNECTION) << 1); //0000 00X0   desloca a esquerda o valor lido e faz or com o tx_data. Ex: Lido(01) << 1 = 10
		systemDio_data = systemDio_data | (digitalRead(PIN_VIBRATION_R) << 2);
		systemDio_data = systemDio_data | (digitalRead(PIN_VIBRATION_L) << 3);
		systemDio_data = systemDio_data | (digitalRead(PIN_BUZZER) << 4);
	}
	
	if(millis() - updateRangerLastPrd >= UPDATE_RANGER_FRQ){
		updateRangerLastPrd = millis();
		ranger_update();
	}

	if(millis() - updateBatteryLastPrd >= UPDATE_BATTERY_FRQ){
		updateBatteryLastPrd = millis();
		battery_update();
	}

	bumper_update();

	//Debug Timestamp
	/*if((bumper[0]==1 or bumper[1]==1 or bumper[2]==1) && bumperTsFlag==0 & motorRActive==1 && motorLActive==1){ 
		bumperTs = millis();
		bumperTsFlag=1;
	}
	if(motorRActive==0 && motorLActive==0 && bumperTsFlag ==1){
		bumperTs = millis() - bumperTs;
		systemMsg(String("BumperTs:")+String(bumperTs)+"ms\n");
		bumperTsFlag=0;
	}*/
	/*if(range[1] <= 15 && rangerTsFlag==0 & motorRActive==1 && motorLActive==1){ 
		rangerTs = millis();
		rangerTsFlag=1;
	}
	if(motorRActive==0 && motorLActive==0 && rangerTsFlag ==1){
		rangerTs = millis() - rangerTs;
		systemMsg(String("Front Ranger:")+String(range[1])+"cm,"+String("Ts:")+String(rangerTs)+"ms\n");
		rangerTsFlag=0;
	}*/
	//#Debug Timestamp
}


void sendData(){
	int i;

	if(millis() - sendDioLastPrd >= SEND_DIO_FRQ){
		sendDioLastPrd = millis();
		sendSystemDioMsg(systemDio_data,5); //value(ex: 00 1110 0011), bitfield(ex: 10)
	}
	if(millis() - sendRangerLastPrd >= SEND_RANGER_FRQ){
		sendRangerLastPrd = millis();
		sendRangerMsg(range,7);
	}
	if(millis() - sendBumperLastPrd >= SEND_BUMPER_FRQ){
		sendBumperLastPrd = millis();
		sendBumperMsg(bumper,6);
	}
	if(millis() - sendBatteryLastPrd >= SEND_BATTERY_FRQ){
		sendBatteryLastPrd = millis();
		sendPowerMsg(battery);
		int rawValue = analogRead(PIN_BATTERY); //read the analogic input
		float volts = map(rawValue,ANALOG_EQUIV_EMPTY,ANALOG_EQUIV_FULL,BATTERY_EMPTY*10,BATTERY_FULL*10)/10.0;
		//systemMsg("Analog Read:");
		//systemMsg(String(rawValue));
		//systemMsg(String(volts));
	}
	if(millis() - sendPingLastPrd >= SEND_PING_FRQ){
		sendPingLastPrd = millis();
		if(ping_flag){
			sendPing();
			lostConnectionFlag = 1;
		}
		else {
			sendPing();
			ping_flag = 1;
		}
	}
	if(millis() - sendEncoderLastPrd >= SEND_ENCODER_FRQ){
		//sendEncoderLastPrd = millis();  //debug timestamp
		//encoderTimestamp = micros() - encoderTimestamp; //debug timestamp
		sendEncoderMsg(counterR,counterL,m.getSpeedR(),m.getSpeedL());
		//systemMsg(String("Ec:")+String(encoderTimestamp)+",us\n"); //debug timestamp
		//encoderTimestamp = micros(); //debug timestamp
		//systemMsg(String("Enc_R:")+String(counterR));
		//systemMsg(String("|Enc_L:")+String(counterL)+"\n");
	}
	if(millis() - sendSystemMsgLastPrd >= SEND_SYSTEMMSG_FRQ){
		sendSystemMsgLastPrd = millis();
		//systemMsg("Mensagem default");
		//convertToUint8_t(battery,2);
	}

	//if(millis() - sendRangerLastPrd >= SEND_RANGER_FRQ == 0) sendEncoderTicks(){
	//sendRangerLastPrd = millis();
	//}	
}

void driver_config(){
	neckServo.attach(PIN_SERVO_NECK);
  neckServo.write(90);
	Timer3.initialize(10000); //10ms = 10000
	Timer3.attachInterrupt(updateActuators);

	waitingConfigFlag = 1;
	//blink fast when no config received yet
	do{
		if(millis() - sendRequestLastPrd >= SEND_REQUESTCONFIG_FRQ){
			sendRequestLastPrd = millis();
			sendRequestConfig();
		}
		readCommand();
		battery_update();
		updateAlerts();
		//updateCounter(); 
	}while(!receivedConfigCmd());
	tone(PIN_BUZZER, 988, 100);
	waitingConfigFlag = 0;
	//updateConfigParameters()
}

void setup(){
	protocol_config();
	status_config();
    driver_config(); //after status_config because led connection
	ranger_config();
	bumper_config();
	motors_config();
}

//!timer3 interruption
/*!update the moviment of motors.*/
void updateActuators(){
	motorsUpdate();   //update pid, must be call in a fix amount of time (10 ms in this case)
	//counter+=10;  //each counter means 10ms
}


void loop(){
	//deal with the incoming data from driver
	readCommand();

	//Determines what to do acording the message type
	processCommand(); 
	
	//read each sensor and write in the driver with the correct message type.
	updateSensors();  

 	//update leds, vibs and buzzer indicators
	updateAlerts();

	//send internal data to driver
	sendData();
}
