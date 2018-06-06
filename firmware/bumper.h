#ifndef bumper_h
#define bumper_h

#include <Arduino.h>

uint8_t bumper[6];

void bumper_config() {
    pinMode(PIN_BUMPER_NE, INPUT_PULLUP);
    pinMode(PIN_BUMPER_NW, INPUT_PULLUP);
    pinMode(PIN_BUMPER_SW, INPUT_PULLUP);
    pinMode(PIN_BUMPER_SE, INPUT_PULLUP);
}

void bumper_update() {
    if (!digitalRead(PIN_BUMPER_NE) && !digitalRead(PIN_BUMPER_NW)) {
	bumper[0] = 0;
	bumper[1] = 1;  //N
	bumper[2] = 0;
    } else {
	bumper[0] = digitalRead(PIN_BUMPER_NE) ? 0 : 1;  //if pin is 1 return 0 else return 1
	bumper[1] = 0;					 //N
	bumper[2] = digitalRead(PIN_BUMPER_NW) ? 0 : 1;
    }

    if (!digitalRead(PIN_BUMPER_SW) && !digitalRead(PIN_BUMPER_SE)) {
	bumper[3] = 0;
	bumper[4] = 1;  //S
	bumper[5] = 0;
    } else {
	bumper[3] = digitalRead(PIN_BUMPER_SE) ? 0 : 1;  //if pin is 1 return 0 else return 1, inverted because is in robot backward.
	bumper[4] = 0;					 //S
	bumper[5] = digitalRead(PIN_BUMPER_SW) ? 0 : 1;
    }
}

#endif
