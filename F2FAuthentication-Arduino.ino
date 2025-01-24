
#include "globals.h"



/** 
* Serial bus setup for communication to IDE and SIM800L
**/
//HardwareSerial *gGSMModemBus = &Serial1;
// Later we want to cast the gGSMModemBus object to Adafruit_FONA to have some nice GSM AT commands integrated in functions
//Adafruit_FONA sim800l = Adafruit_FONA(SIM800L_PWRKEY); 
GSMModem* gModem = NULL; 


/**
* Display
**/
F2FADisplay* gDisplay = NULL; 


/**
* Constants maybe necessary
**/
char simPIN[]   = SIM_PIN; // SIM card PIN code, if any

/**
* Special Runtime feature toggles
**/
// if there is the need to reset the modem
//#define RESET_SIM800 


/**
* Loop timer
**/
unsigned long gSignalStrengthPrevMillis = 0;
const long gSignalStrengthInterval = 10000; 
SignalStrength gLastSignalStrength = SignalStrength::zero;

unsigned long gTimePrevMillis = 0;
const long gTimeInterval = 5000; // Interval 5 seconds


unsigned long gSMSPollPrevMillis = 0;
const long gSMSPollInterval = 10000; // Interval 5 seconds

/**
* Some runtime status information
**/
DeviceState* gDeviceState = new DeviceState();

/**
* Slack and Wifi
**/
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
Slack* gSlack = new Slack(SLACK_WEBHOOK_URL);



void ladln(const String &text, bool toDisplay = true) {
  Serial.println(text);

  if (toDisplay) {
    gDisplay->clearRectOnDisplay(0,8,128,32);
    gDisplay->setCursor(0, 8);
    gDisplay->println(text);
    gDisplay->display();
  }
}

void lad(const String &text, bool toDisplay = true) {
  Serial.print(text);

  if (toDisplay) {
    gDisplay->clearRectOnDisplay(0,8,128,32);
    gDisplay->setCursor(0, 8);
    gDisplay->print(text);
    gDisplay->display();
  }
}

void ladln(int number, bool toDisplay = true) {
  Serial.println(number);

  if (toDisplay) {
    gDisplay->clearRectOnDisplay(0,8,128,32);
    gDisplay->setCursor(0, 8);
    gDisplay->println(number);
    gDisplay->display();
  }
}

void lad(int number, bool toDisplay = true) {
  Serial.print(number);

  if (toDisplay) {
    gDisplay->clearRectOnDisplay(0,8,128,32);
    gDisplay->setCursor(0, 8);
    gDisplay->print(number);
    gDisplay->display();
  }
}

}


/****
* SETUP
*****/
void setup() {

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
    ladln(F("#### F2FA on ESP32 with GSM SIM800L ####"), false);
  } else {
    // it's an error, ok, but we still can proceed without display
  }
  
  // we want to see that ESP32 is on, so lets switch on the blue MC LED
  pinMode(LED_BLUE, OUTPUT); 
  digitalWrite(LED_BLUE, HIGH);

  GSMModemInfo modemInfo = GSMModemInfo(); // we are using the defaults
  gModem = new GSMModem(&Serial1, modemInfo);

  /*// thats the pin to activate the SIM800
  pinMode(SIM800L_POWER, OUTPUT); 
  // Keep reset PIN high, thats required in case you want to reset the sim800, you have to set to low
  pinMode(SIM800L_RST, OUTPUT);
  digitalWrite(SIM800L_RST, HIGH);
  */

  /*// you want reset the sim800?
  #ifdef RESET_SIM800
    digitalWrite(SIM800L_RST, LOW);
    delay(100);
    digitalWrite(SIM800L_RST, HIGH); 
    delay(3000); // wait for restart
  #endif
  */
  
  // now lets switch on the SIM800 modem
  ladln(F("Initalising GSM\r\nmodem..."));
  /*digitalWrite(SIM800L_POWER, HIGH);

  //Begin serial communication with Arduino and SIM800L
  gGSMModemBus->begin(BAUDRATE_SMS_CALL);
  while (!gGSMModemBus) {
    ; // wait for connection object port
  }
  */

  ladln("Wait for GSM modem...", false);
 /* unsigned long timeout = 10000;
  if (waitForGSMModem(timeout) == true) {
    gDeviceState->set(EDeviceState::modem, ON);
    ladln("GSM modem is ready");

    if (!sim800l.begin(*gGSMModemBus)) { // Adafruit_FONA is starting now some standard routines on the GSM modem
      ladln(F("Couldn't find\r\nGSM SIM800L"));
      // TODO ERROR
      while (1); // stay here
    }

    ladln("Connecting GSM\r\nnetwork...");
    if (gDeviceState->get(EDeviceState::modem) == true && waitForNetwork(WaitForGSMNetWorkTimeout)) { // wait upto 30 seconds
      gDeviceState->set(EDeviceState::network, ON);
      ladln("GSM Network\r\nconnected.");
      updateSignalStrengthIfNeeded();

      if (sim800l.enableNetworkTimeSync(true) == true) {
        Serial.println(F("Enabled GSM network time"));
        gDeviceState->set(EDeviceState::networkTime, ON);
      } else {
        Serial.println(F("Failed to enable Network Time"));
        // TODO ERROR
      }
    } else {
      ladln("GSM Network\r\nnot available.");
      // TODO ERROR
    }
  } else {
    // TODO ERROR
    ladln("GSM modem can\r\nnot be activated!");
  }
  */

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

   
  #ifdef DEBUG
    printMenu();
  #endif


  // removing all texts, before handing over to main llop
  ladln(""); 

  // send information to slack that device has startet

  String slackMessage = "══════════\\r\\nF2FA phone has started with follwoing states:\\r\\n──────────\\r\\n";
  slackMessage += gDeviceState->getDescription();
  slackMessage += "══════════";
  ladln(slackMessage, false);
  gSlack->sendMessage(slackMessage, false);

}

void forwardFromUart()
{
  delay(500);
  while (SerialIDE.available()) 
  {
    //SerialIDE.print("Typed by User on MC Serial: "); 
    SerialIDE.write(SerialIDE.read()); 
    SerialIDE.println("");
     sim800l.write(SerialIDE.read());//Forward what Serial received to Software Serial Port
  }
  while(sim800l.available()) 
  {
    //SerialIDE.print("Coming from SIM800L: ");
    SerialIDE.write(sim800l.read());//Forward what Software Serial received to Serial Port
  }
}

void updateSignalStrengthIfNeeded(){
  SignalStrength sgnStrngth = gModem->getSignalStrength();

  if (sgnStrngth != gLastSignalStrength) {
    Serial.print(F("New Signal Strength")); Serial.println(sgnStrngth);
    gDisplay->updateIconSignalStrength((F2FADisplay::DisplaySignalStrength)sgnStrngth);
    gLastSignalStrength = sgnStrngth;
  }
}


void updateCurrentTime(){
    char timeBuffer[23];
    gModem->getTime(timeBuffer, 23);  // getting the time via FONA from GSM modem
    Serial.print(F("Time = ")); Serial.println(timeBuffer);

    // if modem is not fully initalized, we're getting not valid date starting at 04/01/01, we ignore this
    const char* searchString = "04/01/01";
    char* result = strstr(timeBuffer, searchString);

    if (result != NULL) {
      Serial.print(F("Time = ")); Serial.print(timeBuffer);  Serial.print(F("++++NOT VALID"));
    } else {
      gDisplay->updateTextWithCurrentTime(timeBuffer);
    }
}


void forwardAndDeleteSMSIfNeeded() {
  int8_t countSms = gModem->getCountSMS();
  gDisplay->updateIconMessage(countSms);

  if (countSms > 0) {
      Serial.print(F("Found ")); Serial.print(countSms); Serial.println(F(" new SMS's on SIM card!"));

      // open each SMS 
      uint16_t smslen;
      int8_t smsn = 1;
      char smsTextBuffer[255];

      for ( ; smsn <= countSms; smsn++) {
        Serial.print(F("\n\rReading SMS #")); Serial.println(smsn);
        if (!gModem->readSMS(smsn, smsTextBuffer, 250, &smslen)) {  // pass in buffer and max len!
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

        Serial.print(F("***** SMS #")); Serial.print(smsn);
        Serial.print(" ("); Serial.print(smslen); Serial.println(F(") bytes *****"));
        Serial.println(smsTextBuffer);
        Serial.println(F("*****"));

        String slackMessange = "Received new SMS:\r\n'" + String(smsTextBuffer) + "'"; 
        if (gSlack->sendMessage(String(slackMessange)) == true ) {
          // delete SMS
          Serial.print(F("\n\rDeleting SMS #")); Serial.println(smsn);
          if (gModem->deleteSMS(smsn)) {
            Serial.println(F(" Successful deleted!"));
          } else {
            Serial.println(F("Couldn't delete"));
          }
        } else {
          Serial.println(F("ERROR: SMS not deleted because of error during send to Slack"));
        }

      }
    } 
}


/**
* Main Loop
**/
void loop() {

  #ifdef DEBUG
    debugLoop();
  #else
    unsigned long currentMillis = millis();

  // is modem is on ? 


  // get the signal strength and update the diplay if it has changed
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

  #endif
  

}


