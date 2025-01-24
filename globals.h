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
#define LED_BLUE  13 // pin of blue LED of the ESP32

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