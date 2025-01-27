#ifndef globals_h
#define globals_h

#include <Arduino.h>
#include <WiFi.h>
#include "GSMModem.h"
#include "environment_secrets.h"
#include "slack.h"
#include "deviceState.h"
#include "Display.h"

#define F2FA_VERSION "v0.1"

/**
* HARDWARE SETUP
**/
#define LED_BLUE  13 // pin of blue LED of the ESP32
#define SerialIDE Serial0

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