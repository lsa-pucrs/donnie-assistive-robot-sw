#include "sensors.h"


Ranger::Ranger(uint8_t echoPin,uint8_t trigPin){
	setMaxRange(200);
	setMinRange(0);
	setEchoPin(echoPin);
	setTrigPin(trigPin);
}

uint8_t Ranger::getData(){return range;}
uint8_t Ranger::getRange(){return range;}
uint8_t Ranger::getMaxRange(){return maxRange;}
uint8_t Ranger::getMinRange(){return minRange;}
uint8_t Ranger::getEchoPin(){return echoPin;}
uint8_t Ranger::getTrigPin(){return trigPin;}

void Ranger::setMaxRange(uint8_t maxRange){this->maxRange=maxRange;}
void Ranger::setMinRange(uint8_t minRange){this->minRange=minRange;}
void Ranger::setEchoPin(uint8_t echoPin){this->echoPin=echoPin;}
void Ranger::setTrigPin(uint8_t trigPin){this->trigPin=trigPin;}
void Ranger::setRange(uint8_t newRange){this->range=newRange;}

int Ranger::update(){
	uint16_t duration; // Duration used to calculate distance
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);

	/* The following trigPin/echoPin cycle is used to determine the
	distance of the nearest object by bouncing soundwaves off of it. */ 
	digitalWrite(trigPin, LOW); 
	delayMicroseconds(2); 

	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10); 

	digitalWrite(trigPin, LOW);
	duration = pulseIn(this->echoPin, HIGH);

	//Calculate the distance (in cm) based on the speed of sound.
	range = duration/58.2; //g

	///* Send a negative number to computer to indicate "out of range" */
	if (range > maxRange) return 1;
	else if(range < minRange)return -1;
	else return 0;
}
