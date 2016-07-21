#include <Arduino.h>

typedef unsigned char uint8_t;

class Ranger{
    public:
        Ranger(uint8_t echoPin,uint8_t trigPin);

        uint8_t getData();
        uint8_t getRange();
        uint8_t getMaxRange();//[cm]
        uint8_t getMinRange();//[cm]
        uint8_t getEchoPin();
        uint8_t getTrigPin();


        void setMaxRange(uint8_t maxRange);   //[cm]
        void setMinRange(uint8_t minRange);   //[cm]
        void setEchoPin(uint8_t echoPin);
        void setTrigPin(uint8_t trigPin);

        int update();

    private:
        void setRange(uint8_t newRange);

        uint8_t range; //[cm]-Centimeters

        uint8_t maxRange; // Maximum range needed
        uint8_t minRange; // Minimum range needed

        uint8_t trigPin; //OUTPUT
        uint8_t echoPin;//INPUT

        //int8_t active; //
        //int8_t echoState;
};
