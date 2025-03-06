#include "TrafficLight.h"


TrafficLight::TrafficLight(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin) {
  this->redPin = redPin;
  this->yellowPin = yellowPin;
  this->greenPin = greenPin;

  pinMode(this->redPin, OUTPUT);
  pinMode(this->yellowPin, OUTPUT);
  pinMode(this->greenPin, OUTPUT);

  this->redOff();
  this->yellowOff();
  this->greenOff();

}



/**
* called from main loop function
**/
void TrafficLight::loop() {

  unsigned long currentMillis = millis();

  if (this->redBlinkIntervall > 0 && currentMillis - this->redPreviousMillis >= this->redBlinkIntervall) {
    this->redPreviousMillis = currentMillis; 

    this->redLEDState = !this->redLEDState;
    digitalWrite(this->redPin, this->redLEDState );
  }

  if (this->yellowBlinkIntervall > 0 && currentMillis - this->yellowPreviousMillis >= this->yellowBlinkIntervall) {
    this->yellowPreviousMillis = currentMillis; 

    this->yellowLEDState = !this->yellowLEDState;
    digitalWrite(this->yellowPin, this->yellowLEDState );
  }

  if (this->greenBlinkIntervall > 0 && currentMillis - this->greenPreviousMillis >= this->greenBlinkIntervall) {
    this->greenPreviousMillis = currentMillis; 

    this->greenLEDState = !this->greenLEDState;
    digitalWrite(this->greenPin, this->greenLEDState );
  }


}

/** RED **/

bool TrafficLight::switchRedLED() {
  if (this->redLEDState == HIGH) {
    this->redOff();
    Serial.println("++++++++ RED LED IS OFF");
    return false;
  } else {
    this->redOn();
    Serial.println("++++++++ RED LED IS ON");
    return true;
  }
}

void TrafficLight::redOn() {
  if (this->redBlinkIntervall == 0) {
    this->redLEDState = HIGH;
    digitalWrite(this->redPin, this->redLEDState); 
  } else {
    // will be handled in Loop timer function
  }
}

void TrafficLight::redBlinking(long intervall) {
  this->redBlinkIntervall = intervall;
}

void TrafficLight::redOff() {
  this->redLEDState = LOW;
  digitalWrite(this->redPin, this->redLEDState); 
}

/** YELLOW **/

bool TrafficLight::switchYellowLED() {
  if (this->yellowLEDState == HIGH) {
    this->yellowOff();
    Serial.println("++++++++ Yellow LED IS OFF");
    return false;
  } else {
    this->yellowOn();
    Serial.println("++++++++ Yellow LED IS ON");
    return true;
  }
}

void TrafficLight::yellowOn() {
  if (this->yellowBlinkIntervall == 0) {
    this->yellowLEDState = HIGH;
    digitalWrite(this->yellowPin, this->yellowLEDState); 
  } else {
    // will be handled in Loop timer function
  }
}

void TrafficLight::yellowBlinking(long intervall) {
  this->yellowBlinkIntervall = intervall;
}

void TrafficLight::yellowOff() {
  this->yellowLEDState = LOW;
  digitalWrite(this->yellowPin, this->yellowLEDState); 
}


/** GREEN **/

bool TrafficLight::switchGreenLED() {
  if (this->greenLEDState == HIGH) {
    this->greenOff();
    Serial.println("++++++++ Green LED IS OFF");
    return false;
  } else {
    this->greenOn();
    Serial.println("++++++++ Green LED IS ON");
    return true;
  }
}


void TrafficLight::greenOn() {
  if (this->greenBlinkIntervall == 0) {
    this->greenLEDState = HIGH;
    digitalWrite(this->greenPin, this->greenLEDState); 
  } else {
    // will be handled in Loop timer function
  }
}

void TrafficLight::greenBlinking(long intervall) {
  this->greenBlinkIntervall = intervall;
}


void TrafficLight::greenOff() {
  this->greenLEDState = LOW;
  digitalWrite(this->greenPin, this->greenLEDState); 
}


