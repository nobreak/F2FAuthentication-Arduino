#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <Arduino.h>

class TrafficLight {
  public:
    // Constructor
    TrafficLight(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin);
    

    bool switchRedLED();
    bool switchYellowLED();
    bool switchGreenLED();
    
  private:
    void redOn();
    void redOff();
    bool isRedIsOn = true;

    void yellowOn();
    void yellowOff();
    bool isYellowIsOn = true;

    void greenOn();
    void greenOff();
    bool isGreenIsOn = true;


    uint8_t redPin;
    uint8_t yellowPin;
    uint8_t greenPin;
    
};




#endif