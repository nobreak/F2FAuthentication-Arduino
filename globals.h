#ifndef globals_h
#define globals_h

#include <Arduino.h>
#include <WiFi.h>
#include "GSMModem.h"
#include "environment_secrets.h"
#include "slack.h"
#include "deviceState.h"
#include "Display.h"
#include "TrafficLight.h"

#define F2FA_VERSION "v0.1.1"
//#define TTGO_TCALL

/**
* HARDWARE SETUP
**/
#define LED_BLUE  13 // pin of blue LED of the ESP32
#define SerialIDE Serial0

/**
* Traffic Light
**/
#define TL_RED_LED 14
#define TL_YELLOW_LED 27
#define TL_GREEN_LED 26

/**
* GSM Modem
**/
#ifdef TTGO_TCALL
  #define MODEM_TX 27 
  #define MODEM_RX 26 
  #define MODEM_RST 5 
  #define MODEM_PWR_KEY 4 
  #define MODEM_POWER 23
#else
  #define MODEM_TX 17 
  #define MODEM_RX 16 
  // TODO TBD:
  #define MODEM_RST 5 
  #define MODEM_PWR_KEY 4
  #define MODEM_POWER 23
#endif


/**
* Display
**/
extern F2FADisplay* gDisplay;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

/**
* Timings
**/
#define WaitForGSMNetWorkTimeout 30000 // 30 s
#define ReadDelay 2000 // how long you need to read some message on display

#endif