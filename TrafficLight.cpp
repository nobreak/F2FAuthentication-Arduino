#include "TrafficLight.h"


TrafficLight::TrafficLight(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin) {
  this->redPin = redPin;
  this->yellowPin = yellowPin;
  this->greenPin = greenPin;

  pinMode(this->redPin, OUTPUT);
  pinMode(this->yellowPin, OUTPUT);
  pinMode(this->greenPin, OUTPUT);

}

bool TrafficLight::switchRedLED() {
  if (this->isRedIsOn == true) {
    this->redOff();
    Serial.println("++++++++ RED LED IS OFF");
    return false;
  } else {
    this->redOn();
    Serial.println("++++++++ RED LED IS ON");
    return true;
  }
}

bool TrafficLight::switchYellowLED() {
  if (this->isYellowIsOn == true) {
    this->yellowOff();
    Serial.println("++++++++ Yellow LED IS OFF");
    return false;
  } else {
    this->yellowOn();
    Serial.println("++++++++ Yellow LED IS ON");
    return true;
  }
}

bool TrafficLight::switchGreenLED() {
  if (this->isGreenIsOn == true) {
    this->greenOff();
    Serial.println("++++++++ Green LED IS OFF");
    return false;
  } else {
    this->greenOn();
    Serial.println("++++++++ Green LED IS ON");
    return true;
  }
}


void TrafficLight::redOn() {
  digitalWrite(this->redPin, LOW); // LOW means ON for the common anode
  this->isRedIsOn = true;
}

void TrafficLight::redOff() {
  digitalWrite(this->redPin, HIGH); // HIGH means OFF for the common anode
  this->isRedIsOn = false;
}


void TrafficLight::yellowOn() {
  digitalWrite(this->yellowPin, LOW); // LOW means ON for the common anode
  this->isYellowIsOn = true;
}

void TrafficLight::yellowOff() {
  digitalWrite(this->yellowPin, HIGH); // HIGH means OFF for the common anode
  this->isYellowIsOn = false;
}

void TrafficLight::greenOn() {
  digitalWrite(this->greenPin, LOW); // LOW means ON for the common anode
  this->isGreenIsOn = true;
}

void TrafficLight::greenOff() {
  digitalWrite(this->greenPin, HIGH); // HIGH means OFF for the common anode
  this->isGreenIsOn = false;
}


