#ifndef ranger_h
#define ranger_h

#include <Arduino.h>
//#include "Ranger.h"

//#ifndef RANGER_H
//#define RANGER_H

void ranger_config();

void ranger_update();

void ranger_print();

void GetRange1();

void GetRange2();

void GetRange3();

void SetTrig();

//#endif

//#include <Arduino.h>
//#include "Ranger.h"

#define echo0 19
#define echo1 20
#define echo2 21

int trig = 0;

volatile int trigSet = 0;
const int trigs[4] = {22, 24, 26, 23};
uint8_t range[7];
unsigned long trigTime;
unsigned long trigFreq;

void ranger_config() {
    pinMode(echo0, INPUT);
    pinMode(echo1, INPUT);
    pinMode(echo2, INPUT);

    attachInterrupt(2, GetRange1, FALLING);
    attachInterrupt(3, GetRange2, FALLING);
    attachInterrupt(4, GetRange3, FALLING);

    trigFreq = micros();
}

void ranger_update() {
    if (micros() >= trigFreq + 50000) {
	SetTrig();
	trigFreq = micros();
    }
}

void ranger_print() {
    Serial.print(range[0]);
    Serial.print("   ");
    Serial.print(range[1]);
    Serial.print("   ");
    Serial.print(range[2]);
    Serial.print("   ");
    Serial.print(range[3]);
    Serial.print("   ");
    Serial.print(range[4]);
    Serial.print("   ");
    Serial.print(range[5]);
    Serial.print("   ");
    Serial.print(range[6]);
    Serial.println("   ");
}

void GetRange1() {
    //if (trig != 3){
    range[trig] = (micros() - trigTime - 450) / 58.2;
    //trigSet--;
    //}
}

void GetRange2() {
    //if (trig != 3){
    range[trig + 3] = (micros() - trigTime - 450) / 58.2;
    //trigSet--;
    //}
}

void GetRange3() {
    //if (trigSet == 3){
    range[6] = (micros() - trigTime - 450) / 58.2;
    //}
}

void SetTrig() {
    trig++;
    if (trig == 4)
	trig = 0;
    pinMode(trigs[trig], OUTPUT);

    digitalWrite(trigs[trig], LOW);
    delayMicroseconds(2);
    digitalWrite(trigs[trig], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigs[trig], LOW);
    trigTime = micros();
    //trigSet = 2;
}

#endif
