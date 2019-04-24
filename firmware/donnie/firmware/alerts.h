#ifndef status_h
#define status_h

#include <Arduino.h>
#include "messages.h"
#include "motors.h"

float battery; //TODO deixar como int32_t
uint8_t lostConnectionFlag = 0;
uint8_t batteryCriticalFlag = 0;
uint8_t waitingConfigFlag = 0;



//uint8_t lowPowerAlertFlag = 0;
//int8_t beepCounts = 0;

//TODO criar struct para low_battery_alert_beep

typedef struct{
    uint32_t state = 0;
    uint32_t time,wait;
    uint32_t lastTime = 0;
} noWaitDelay;




noWaitDelay low_battery_alert_vibration_timer_r;
noWaitDelay low_battery_alert_buzzer_timer;

noWaitDelay low_battery_sequence;
noWaitDelay connection_lost_sequence;
noWaitDelay waiting_config_sequence;


//batteryCheck()
//connectionCheck
//ping()  //sent a message to player and mark the time to get the response

void battery_update();
void connection_update(uint32_t counter);
void lowBatteryAlertBeep();
//void lowPowerAlertVibration();
int batteryCriticalCheck();

void resetLowBatteryAlerts();


void status_config();
void lowBatterySequence();
void connectionLostSequence();
void waitingConfigSequence();
void updateAlerts();


void status_config(){
	pinMode(PIN_LED_BATTERY, OUTPUT);
	pinMode(PIN_LED_CONNECTION, OUTPUT);
	pinMode(PIN_VIBRATION_R, OUTPUT);
	pinMode(PIN_VIBRATION_L, OUTPUT);
	pinMode(PIN_BUZZER, OUTPUT);

	digitalWrite(PIN_LED_BATTERY,HIGH);
	digitalWrite(PIN_LED_CONNECTION,HIGH);


	
	tone(PIN_BUZZER, 523, 100);
	digitalWrite(PIN_VIBRATION_R, HIGH);
	delay(200);
	tone(PIN_BUZZER, 988, 100);
	digitalWrite(PIN_VIBRATION_R, LOW);
	delay(500);
	digitalWrite(PIN_VIBRATION_L, HIGH);
	delay(200);
	digitalWrite(PIN_VIBRATION_L, LOW);
}

void battery_update(){
	int rawValue = analogRead(PIN_BATTERY); //read the analogic input
	battery = map(rawValue,ANALOG_EQUIV_EMPTY,ANALOG_EQUIV_FULL,BATTERY_EMPTY*10,BATTERY_FULL*10)/10.0;
	batteryCriticalFlag = batteryCriticalCheck();
}

void lowBatterySequence(){
	low_battery_sequence.time = millis();
	low_battery_sequence.wait = low_battery_sequence.time - low_battery_sequence.lastTime;

	switch(low_battery_sequence.state){
		case 0: 
			digitalWrite(PIN_LED_BATTERY, HIGH);
			low_battery_sequence.state++;
			break;
		case 1: 
			if(low_battery_sequence.wait >= 300){
				low_battery_sequence.lastTime = low_battery_sequence.time;
				low_battery_sequence.state++;
			}
			break;
		case 2: 
			digitalWrite(PIN_LED_BATTERY, LOW);
			low_battery_sequence.state++;
			break;
		case 3: 
			if(low_battery_sequence.wait >= 300){
				low_battery_sequence.lastTime = low_battery_sequence.time;
				low_battery_sequence.state++;
			}
			break;
		case 4: 
			digitalWrite(PIN_LED_BATTERY, HIGH);
			low_battery_sequence.state++;
			break;
		case 5: 
			if(low_battery_sequence.wait >= 300){
				low_battery_sequence.lastTime = low_battery_sequence.time;
				low_battery_sequence.state++;
			}
			break;
		case 6: 
			digitalWrite(PIN_LED_BATTERY, LOW);
			low_battery_sequence.state++;
			break;
		case 7: 
			if(low_battery_sequence.wait >= 300){
				low_battery_sequence.lastTime = low_battery_sequence.time;
				low_battery_sequence.state=0;
			}
			break;
		}
}

void waitingConfigSequence(){
	waiting_config_sequence.time = millis();
	waiting_config_sequence.wait = waiting_config_sequence.time - waiting_config_sequence.lastTime;

	switch(waiting_config_sequence.state){
		case 0: 
			digitalWrite(PIN_LED_CONNECTION, HIGH);
			waiting_config_sequence.state++;
			break;
		case 1: 
			if(waiting_config_sequence.wait >= 150){
				waiting_config_sequence.lastTime = waiting_config_sequence.time;
				waiting_config_sequence.state++;
			}
			break;
		case 2: 
			digitalWrite(PIN_LED_CONNECTION, LOW);
			waiting_config_sequence.state++;
			break;
		case 3: 
			if(waiting_config_sequence.wait >= 150){
				waiting_config_sequence.lastTime = waiting_config_sequence.time;
				waiting_config_sequence.state++;
			}
			break;
		case 4: 
			digitalWrite(PIN_LED_CONNECTION, HIGH);
			waiting_config_sequence.state++;
			break;
		case 5: 
			if(waiting_config_sequence.wait >= 150){
				waiting_config_sequence.lastTime = waiting_config_sequence.time;
				waiting_config_sequence.state++;
			}
			break;
		case 6: 
			digitalWrite(PIN_LED_CONNECTION, LOW);
			waiting_config_sequence.state++;
			break;
		case 7: 
			if(waiting_config_sequence.wait >= 500){
				waiting_config_sequence.lastTime = waiting_config_sequence.time;
				waiting_config_sequence.state=0;
			}
			break;
		}
}

void connectionLostSequence(){
  stop(); //Stops the robot wheels
  
	connection_lost_sequence.time = millis();
	connection_lost_sequence.wait = connection_lost_sequence.time - connection_lost_sequence.lastTime;

	switch(connection_lost_sequence.state){
		case 0: 
			digitalWrite(PIN_LED_CONNECTION, HIGH);
			connection_lost_sequence.state++;
			break;
		case 1: 
			if(connection_lost_sequence.wait >= 700){
				connection_lost_sequence.lastTime = connection_lost_sequence.time;
				connection_lost_sequence.state++;
			}
			break;
		case 2: 
			digitalWrite(PIN_LED_CONNECTION, LOW);
			connection_lost_sequence.state++;
			break;
		case 3: 
			if(connection_lost_sequence.wait >= 150){
				connection_lost_sequence.lastTime = connection_lost_sequence.time;
				connection_lost_sequence.state++;
			}
			break;
		case 4: 
			digitalWrite(PIN_LED_CONNECTION, HIGH);
			connection_lost_sequence.state++;
			break;
		case 5: 
			if(connection_lost_sequence.wait >= 700){
				connection_lost_sequence.lastTime = connection_lost_sequence.time;
				connection_lost_sequence.state++;
			}
			break;
		case 6: 
			digitalWrite(PIN_LED_CONNECTION, LOW);
			connection_lost_sequence.state++;
			break;
		case 7: 
			if(connection_lost_sequence.wait >= 500){
				connection_lost_sequence.lastTime = connection_lost_sequence.time;
				connection_lost_sequence.state=0;
			}
			break;
		}
}

//deal with alerts output
void updateAlerts(){
	if(batteryCriticalFlag){
		lowBatterySequence();
		//low_battery_alert_beep();
		//lowPowerAlertVibration();
	}
	else{
		digitalWrite(PIN_LED_BATTERY, HIGH);
	}

	if(lostConnectionFlag){
		connectionLostSequence();
	}
	else if(waitingConfigFlag){
		waitingConfigSequence();
	}
	else{
		//good connection
		digitalWrite(PIN_LED_CONNECTION, HIGH);
	}
}

/*
void low_battery_alert_beep(){
	low_battery_alert_buzzer_timer.time = millis();
	low_battery_alert_buzzer_timer.wait = low_battery_alert_buzzer_timer.time - low_battery_alert_buzzer_timer.lastTime;

	switch(low_battery_alert_buzzer_timer.state){
		case 0: 
			digitalWrite(PIN_BUZZER, HIGH);
			low_battery_alert_buzzer_timer.state++;
			break;
		case 1: 
			if(low_battery_alert_buzzer_timer.wait >= 800){
				low_battery_alert_buzzer_timer.lastTime = low_battery_alert_buzzer_timer.time;
				low_battery_alert_buzzer_timer.state++;
			}
			break;
		case 2: 
			digitalWrite(PIN_BUZZER, LOW);
			low_battery_alert_buzzer_timer.state++;
			break;
		case 3: 
			if(low_battery_alert_buzzer_timer.wait >= 200){
				low_battery_alert_buzzer_timer.lastTime = low_battery_alert_buzzer_timer.time;
				low_battery_alert_buzzer_timer.state++;
			}
			break;
		case 4: 
			digitalWrite(PIN_BUZZER, HIGH);
			low_battery_alert_buzzer_timer.state++;
			break;
		case 5: 
			if(low_battery_alert_buzzer_timer.wait >= 800){
				low_battery_alert_buzzer_timer.lastTime = low_battery_alert_buzzer_timer.time;
				low_battery_alert_buzzer_timer.state++;
			}
			break;
		case 6: 
			digitalWrite(PIN_BUZZER, LOW);
			low_battery_alert_buzzer_timer.state++;
			break;
	}
}
void lowPowerAlertVibration(){
	low_battery_alert_vibration_timer_r.time = millis();
	low_battery_alert_vibration_timer_r.wait = low_battery_alert_vibration_timer_r.time - low_battery_alert_vibration_timer_r.lastTime;

	switch(low_battery_alert_vibration_timer_r.state){
		case 0: 
			digitalWrite(PIN_VIBRATION_R, HIGH);
			low_battery_alert_vibration_timer_r.state++;
			break;
		case 1: 
			if(low_battery_alert_vibration_timer_r.wait >= 800){
				low_battery_alert_vibration_timer_r.lastTime = low_battery_alert_vibration_timer_r.time;
				low_battery_alert_vibration_timer_r.state++;
			}
			break;
		case 2: 
			digitalWrite(PIN_VIBRATION_R, LOW);
			low_battery_alert_vibration_timer_r.state++;
			break;
		case 3: 
			if(low_battery_alert_vibration_timer_r.wait >= 200){
				low_battery_alert_vibration_timer_r.lastTime = low_battery_alert_vibration_timer_r.time;
				low_battery_alert_vibration_timer_r.state++;
			}
			break;
		case 4: 
			digitalWrite(PIN_VIBRATION_R, HIGH);
			low_battery_alert_vibration_timer_r.state++;
			break;
		case 5: 
			if(low_battery_alert_vibration_timer_r.wait >= 800){
				low_battery_alert_vibration_timer_r.lastTime = low_battery_alert_vibration_timer_r.time;
				low_battery_alert_vibration_timer_r.state++;
			}
			break;
		case 6: 
			digitalWrite(PIN_VIBRATION_R, LOW);
			low_battery_alert_vibration_timer_r.state++;
			break;
	}
}
*/


int batteryCriticalCheck(){   
	//simulates sterese to avoid multiples alerts by voltage noises
	if(batteryCriticalFlag){
		if(battery<BATTERY_LOW_BATTERY_ALERT+0.3){
			return 1;
		}
		else return 0;
	}
	else{
		if(battery<BATTERY_LOW_BATTERY_ALERT){
			return 1;
		}
		else return 0;
	}
}

/*
void reset_low_battery_alerts(){
	if(low_battery_alert_vibration_timer_r.state!=0){
		low_battery_alert_vibration_timer_r.state=0;
		digitalWrite(PIN_VIBRATION_R, LOW);
		digitalWrite(PIN_VIBRATION_L, LOW);
	}
	if(low_battery_alert_buzzer_timer.state!=0){
		low_battery_alert_buzzer_timer.state=0;
		digitalWrite(PIN_BUZZER, LOW);
	}
}*/



#endif
