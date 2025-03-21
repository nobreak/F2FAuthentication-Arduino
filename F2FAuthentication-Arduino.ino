
#include "globals.h"


// some forward declarations
void ladln(const String &text, bool toDisplay = true);
void updateSignalStrengthIfNeeded();
void sendDeviceStateToSlack(String message);
//class F2FAEventHandler;
void resetDevice();
void forwardAndDeleteSMSIfNeeded();
void updateCurrentTime();
void sendDeviceStateToSlack(String message);
void updateSignalStrengthIfNeeded();
// log and display functions
void ladln(const String &text, bool toDisplay);
void lad(const String &text, bool toDisplay = true);
void ladln(int number, bool toDisplay = true);
void lad(int number, bool toDisplay = true);

/****
* Necessary objects
*****/
GSMModem* gModem = NULL;  // Serial bus setup for communication to IDE and SIM800L
DeviceState* gDeviceState = new DeviceState(); // some states about the device
F2FADisplay* gDisplay = NULL; // our OLED display
TrafficLight* gTrafficLight = NULL; 


class F2FAEventHandler : public GSMModemDelegate {
  public:
    void onModemError(EGSMModemError errorCode, EGSMErrorType errorType, char* description) override {

      String errMsg = "Error(" + String((uint8_t)errorCode) + "): " + String(description);

      if (errorType == Critical || errorType == Normal) {
        ladln(errMsg);
      } else {
        ladln(errMsg, false);
      }

      gDeviceState->addErrorMessage(errMsg);

      switch (errorCode) {
        case ErrorNetworkConnectionTimeout:
          // todo: try to restart
          break;
        case ErrorSignalStrengthZero:
          // update traffic light
          // send message to slack
          sendDeviceStateToSlack("Lost GSM Network connection, signal strength is zero");
          break;
        case ErrorCouldNotDeleteSMS:
          // todo: change traffic light colors
          break;
      }
    }

    void onModemStatusChanged(EGSMModemState changedState, bool newState) override {

        switch (changedState) {
          case modemOnline:
            if (newState == true ) {
              gDeviceState->set(EDeviceState::modem, ON);
              ladln("Modem is ready");
              if (gModem->getState(initializing) == true) {
                delay(ReadDelay);
                ladln("Connecting mobile\r\nnetwork...");
              }
            }
            break;
          case networkConnected:
            if (newState == true) {
              gDeviceState->set(EDeviceState::network, ON);
              ladln("GSM Network\r\nconnected.");
              updateSignalStrengthIfNeeded();
            }
            break;
          case networkTimeEnabled:
            if (newState == true) {
              ladln(F("Enabled GSM network time"), false);
              gDeviceState->set(EDeviceState::networkTime, ON);
            }
            break;
          case initializing: 
            if (newState == false) {
              // nothing for now
            } 
            break;
        }
    }
};

F2FAEventHandler gEventHandler; // is receiving status events from different components


/**
* Loop timer
**/
unsigned long gSignalStrengthPrevMillis = 0;
const long gSignalStrengthInterval = 10000; 


unsigned long gTimePrevMillis = 0;
const long gTimeInterval = 5000; // Interval 5 seconds


unsigned long gSMSPollPrevMillis = 0;
const long gSMSPollInterval = 10000; // Interval 10 seconds

unsigned long gSendStatusPrevMillis = 0;
const long gSendStatusInterval = 600000 * 6 * 24; // 10min *6 * 6 = 24h



/**
* Slack and Wifi
**/
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
Slack* gSlack = new Slack(SLACK_WEBHOOK_URL);





/****
* SETUP
*****/
void setup() {

  gTrafficLight = new TrafficLight(TL_RED_LED, TL_YELLOW_LED, TL_GREEN_LED);
  gTrafficLight->redOn();
  
  
  // to see waht happens in the IDE we need the serial bus at first
  //Begin serial communication with ESP32 and Arduino IDE (Serial Monitor to log into IDE what happens)
  SerialIDE.begin(115200);
  // We dont want to loose any messages after first boot of the device, 
  // so we wait for the object and additionally some time (5s)
  delay(5000);
  while (!SerialIDE) {
    ; // wait for connection object port
  }

  // next we need a display
  gDisplay = new F2FADisplay(SCREEN_WIDTH, SCREEN_HEIGHT, 0x3C);  // check the adresse with your device

  if (gDisplay != NULL || gDisplay->isConnected == true) {
    gDeviceState->set(EDeviceState::display, ON);
    lad(F("#### F2FA Phone - ")); lad(F2FA_VERSION); ladln(" ####");
    delay(ReadDelay); 
  } else {
    // it's an error, ok, but we still can proceed without display
    gDeviceState->set(EDeviceState::display, OFF);
  }
  
  gTrafficLight->yellowOn();
  
  #ifdef TTGO_TCALL
    // we want to see that ESP32 is on, so lets switch on the blue MC LED
    pinMode(LED_BLUE, OUTPUT); 
    digitalWrite(LED_BLUE, HIGH);
  #endif


  // now lets switch on the SIM800 modem
  ladln(F("Initializing\r\nF2FA Phone..."));
  GSMModemInfo modemInfo = GSMModemInfo(); // we are using the defaults
  modemInfo.pinRX = MODEM_RX;
  modemInfo.pinTX = MODEM_TX;
  modemInfo.pinPower = MODEM_POWER;
  modemInfo.pinRST = MODEM_RST;
  modemInfo.pinPwrKey = MODEM_PWR_KEY;
  modemInfo.serialBaudrate = BAUDRATE_SMS_CALL;
  gModem = new GSMModem(&Serial1, modemInfo, &gEventHandler, SIM_PIN);
  gModem->setup();

  gTrafficLight->redOff();
  gTrafficLight->greenOn();

  // connecting Wifi
  delay(1000); // when we try to connect WiFi directly after having a GSM network, often we have not enough amps available on USB
  WiFi.begin(ssid, password);
  ladln("Connecting Wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  
  gDeviceState->set(EDeviceState::wifi, ON);
  gDisplay->updateIconWifi(ON);
  ladln("Wifi Connected", false);

  
  // send information to slack that device has startet
  updateSignalStrengthIfNeeded();
  updateCurrentTime();
  gDeviceState->setCountNotForwardedSMS(gModem->getCountSMS());
  sendDeviceStateToSlack("has started with following states:");

  // removing all texts from display , before handing over to main loop
  ladln(""); 

  gTrafficLight->yellowOff();
}



/**
* Main Loop
**/
void loop() {

  gTrafficLight->loop();
   unsigned long currentMillis = millis();

  // get the signal strength and update the display if it has changed
  if (currentMillis - gSignalStrengthPrevMillis >= gSignalStrengthInterval) {
    gSignalStrengthPrevMillis = currentMillis;
    updateSignalStrengthIfNeeded();
  }
    
  // get the time and update the display
  if (gDeviceState->get(EDeviceState::networkTime) == ON && (currentMillis - gTimePrevMillis >= gTimeInterval)) {
    gTimePrevMillis = currentMillis;
    updateCurrentTime();
  }

  // get the count of received SMS and forward them
  // after it we delete the SMS
  if (gDeviceState->get(EDeviceState::network) == ON && currentMillis - gSMSPollPrevMillis >= gSMSPollInterval) {
    gSMSPollPrevMillis = currentMillis;
    // get count of SMS
    forwardAndDeleteSMSIfNeeded();
  }


  if (currentMillis - gSendStatusPrevMillis >= gSendStatusInterval) {
    gSendStatusPrevMillis = currentMillis;
    sendDeviceStateToSlack("Still alive.");
  }



  /*if (gGSMNetworkConnected == false) {
    ladln("Connecting GSM network ...");
    if (waitForNetwork(30000)) { // wait upto 60 seconds
      gGSMNetworkTimeEnabled = true;
      ladln("GSM Network connected.");
      updateSignalStrengthIfNeeded();
    } else {
      ladln("GSM Network not available.");
    }
  }*/
}


void resetDevice() {
  gModem->reset();
  ESP.restart();  
}


void updateSignalStrengthIfNeeded(){
  SignalStrength sgnStrngth = gModem->getSignalStrength();

  if (sgnStrngth != gDeviceState->getSignalStrength()) {
    Serial.print(F("New Signal Strength")); Serial.println(sgnStrngth);
    gDisplay->updateIconSignalStrength((F2FADisplay::DisplaySignalStrength)sgnStrngth);
    gDeviceState->setSignalStrength(sgnStrngth);
  }
}


void sendDeviceStateToSlack(String message) {
  String slackMessage = "F2FA Phone " + String(F2FA_VERSION) + " - " + message + "\\r\\n──────────\\r\\n";
  slackMessage += gDeviceState->getDescription();
  slackMessage += "──────────";
  ladln(slackMessage, false);
  gSlack->sendMessage(slackMessage, false);
  gDeviceState->deleteAllErrorMessages();
}


void updateCurrentTime(){
    char timeBuffer[23];
    gModem->getTime(timeBuffer, 23);  // getting the time via FONA from GSM modem
    Serial.print(F("Time = ")); Serial.println(timeBuffer);

    gDeviceState->setLastNetworkTime(String(timeBuffer));

    // if modem is not fully initalized, we're getting not valid date starting at 04/01/01, we ignore this
    const char* searchString = "04/01/01";
    char* result = strstr(timeBuffer, searchString);

   /* if (result != NULL) {
      Serial.print(F("Time = ")); Serial.print(timeBuffer);  Serial.print(F("++++NOT VALID"));
    } else {*/
      gDisplay->updateTextWithCurrentTime(timeBuffer);
    //}
}

void forwardAndDeleteSMSIfNeeded() {
  int8_t countSms = gModem->getCountSMS();
  gDeviceState->setCountNotForwardedSMS(countSms);
  gDisplay->updateIconMessage(countSms);

  if (countSms > 0) {
      gTrafficLight->greenBlinking(500);
      Serial.print(F("Found ")); Serial.print(countSms); Serial.println(F(" new SMS's on SIM card!"));

      // open each SMS 
      uint16_t smslen;
      int8_t smsn = 1;
      char smsTextBuffer[255];

      for ( ; smsn <= countSms; smsn++) {
        Serial.print(F("\n\rReading SMS #")); Serial.println(smsn);
        if (!gModem->readSMS(smsn, smsTextBuffer, 561, &smslen)) {  // pass in buffer and max len!
          Serial.println(F("SMS Read Failed!"));
          break;
        }
        // if the length is zero, its a special case where the index number is higher
        // so increase the max we'll look at!
        if (smslen == 0) {
          Serial.println(F("[empty slot]"));
          countSms++;
          continue;
        }

        String decodedUFT8Message = gModem->decodeSMSTextIfNeeded(smsTextBuffer);

        Serial.print(F("***** SMS #")); Serial.print(smsn);
        Serial.print(" ("); Serial.print(smslen); Serial.println(F(") bytes *****"));
        Serial.println(decodedUFT8Message);
        Serial.println(F("*****"));

        String slackMessage = "******* Received new SMS: '" + String(decodedUFT8Message) + "'"; 
        if (gSlack->sendMessage(slackMessage) == true ) {
          // delete SMS
          Serial.print(F("\n\rDeleting SMS #")); Serial.println(smsn);
          if (gModem->deleteSMS(smsn)) {
            Serial.println(F(" Successful deleted!"));
          } 
        } else {          
          gDeviceState->addErrorMessage("ERROR: SMS not deleted because of error during send to Slack");
        }

      }
      gTrafficLight->greenBlinking(0);
      gTrafficLight->greenOn();
    } 
}


void ladln(const String &text, bool toDisplay) {

  Serial.println(text);

  if (toDisplay) {
    gDisplay->clearRectOnDisplay(0,8,128,32);
    gDisplay->setCursor(0, 8);
    gDisplay->println(text);
    gDisplay->display();
  }
}

void lad(const String &text, bool toDisplay) {
  Serial.print(text);

  if (toDisplay) {
    gDisplay->clearRectOnDisplay(0,8,128,32);
    gDisplay->setCursor(0, 8);
    gDisplay->print(text);
    gDisplay->display();
  }
}

void ladln(int number, bool toDisplay) {
  Serial.println(number);

  if (toDisplay) {
    gDisplay->clearRectOnDisplay(0,8,128,32);
    gDisplay->setCursor(0, 8);
    gDisplay->println(number);
    gDisplay->display();
  }
}

void lad(int number, bool toDisplay) {
  Serial.print(number);

  if (toDisplay) {
    gDisplay->clearRectOnDisplay(0,8,128,32);
    gDisplay->setCursor(0, 8);
    gDisplay->print(number);
    gDisplay->display();
  }
}



/*********/





