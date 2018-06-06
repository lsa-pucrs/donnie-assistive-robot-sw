//#include <Arduino.h>
//#include "Ranger.h"
//
//#define echo1 20
//#define echo2 21
//
//int trig = 22;
//
//volatile int trigSet = 0;
//unsigned long trigTime;
//unsigned long trigFreq;
//long range[6];
//
//
//void ranger_config(){
//	pinMode(echo1, INPUT);
//	pinMode(echo2, INPUT);
//
//	attachInterrupt(2, GetRange1, FALLING);
//	attachInterrupt(3, GetRange2, FALLING);
//
//	trigFreq = micros();
//}
//
//void ranger_update(){
//	if (micros() >= trigFreq + 50000){
//		SetTrig();
//		trigFreq = micros();
//	}
//}
//
//void ranger_print(){
//	Serial.print(range[0]);
//	Serial.print("   ");
//	Serial.print(range[1]);
//	Serial.print("   ");
//	Serial.print(range[2]);
//	Serial.print("   ");
//	Serial.print(range[3]);
//	Serial.print("   ");
//	Serial.print(range[4]);
//	Serial.print("   ");
//	Serial.print(range[5]);
//	Serial.println("   ");
//}
//
//void GetRange1(){
//	if (trigSet > 0){
//		range[trig - 22] = (micros() - trigTime - 450) / 58.2;
//		trigSet--;
//	}
//}
//
//void GetRange2(){
//	if (trigSet > 0){
//		range[trig - 19] = (micros() - trigTime - 450) / 58.2;
//		trigSet--;
//	}
//}
//
//void SetTrig(){
//
//	trig++;
//	if (trig == 25)
//		trig = 22;
//	pinMode(trig, OUTPUT);
//
//	digitalWrite(trig, LOW);
//	delayMicroseconds(2);
//	digitalWrite(trig, HIGH);
//	delayMicroseconds(10);
//	digitalWrite(trig, LOW);
//	trigTime = micros();
//	trigSet = 2;
//}
//
