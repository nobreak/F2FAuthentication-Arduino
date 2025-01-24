#ifndef globals_h
#define globals_h

#include <Arduino.h>
#include <WiFi.h>
#include "GSMModem.h"
#include "environment_secrets.h"
#include "slack.h"
#include "deviceState.h"
#include "Display.h"



//#define DEBUG

#ifdef DEBUG
  #include "debug.h"
#endif


/**
* HARDWARE SETUP
**/
// that are the PINS which are hardwired on TTGO to realize the hardware serial bus between ESP32 and SIM800L
/*#define SIM800L_TX 27 
#define SIM800L_RX 26
#define SIM800L_RST 5
#define SIM800L_PWRKEY 4
#define SIM800L_POWER 23
*/

// // choose one of them
// #define BAUDRATE_SMS_CALL 9600
// #define BAUDRATE_GPRS 57600
// #define BAUDRATE_IOT_FIRMWAREUPDATE 115200

#define LED_BLUE  13 // pin of blue LED of the ESP32

/** 
* Serial bus setup for communication to IDE and SIM800L
**/
#define SerialIDE Serial0
extern HardwareSerial *gGSMModemBus; 
// Later we want to cast ther UART bus object to Adafruit_FONA to have some nice GSM AT commands integrated in functions
extern Adafruit_FONA sim800l;


/**
* Display
**/
extern F2FADisplay* gDisplay;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32


/**
* Timer timeouts
**/
#define WaitForGSMNetWorkTimeout 30000 // 30 s


#endif