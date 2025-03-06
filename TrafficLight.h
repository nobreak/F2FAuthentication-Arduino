#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <Arduino.h>

class TrafficLight {
  public:
    // Constructor
    TrafficLight(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin);
    
    void loop(); // called from main loop

    bool switchRedLED();
    void redOn();
    void redBlinking(long intervall); // can only be called when loop is active
    void redOff();

    bool switchYellowLED();
    void yellowOn();
    void yellowBlinking(long intervall); // can only be called when loop is active
    void yellowOff();
    
    bool switchGreenLED();
    void greenOn();
    void greenBlinking(long intervall); // can only be called when loop is active
    void greenOff();

    
  private:
    uint8_t redLEDState = LOW;
    long redBlinkIntervall = 0;  //ms
    long redPreviousMillis = 0;
    uint8_t redPin;

    uint8_t yellowLEDState = LOW;
    long yellowBlinkIntervall = 0; // ms
    long yellowPreviousMillis = 0;
    uint8_t yellowPin;

    uint8_t greenLEDState = LOW;
    long greenBlinkIntervall = 0; // ms
    long greenPreviousMillis = 0;
    uint8_t greenPin;    
};




#endif