#include <Arduino.h>
#include "GSMModem.h"


GSMModem::GSMModem(HardwareSerial* hardwareSerialBus, GSMModemInfo modemInfo) : hwSBSIM800L(modemInfo.pinPwrKey) {
  this->hardwareSerialBus = hardwareSerialBus;
  //this->hwSBSIM800L = Adafruit_FONA(modemInfo.pinPwrKey);
  this->info = modemInfo;

  // thats the pin to activate the SIM800
  pinMode(info.pinPower, OUTPUT); 
  // Keep reset PIN high, thats required in case you want to reset the sim800, you have to set to low
  pinMode(info.pinRST, OUTPUT);
  digitalWrite(info.pinRST, HIGH);


  // you want reset the sim800?
  #ifdef RESET_SIM800
    digitalWrite(pinRST, LOW);
    delay(100);
    digitalWrite(pinRST, HIGH); 
    delay(3000); // wait for restart
  #endif

  // now lets switch on the SIM800 modem
  digitalWrite(info.pinPower, HIGH);

  //Begin serial communication with Arduino and SIM800L
  hardwareSerialBus->begin(info.serialBaudrate);
  while (!hardwareSerialBus) {
    ; // wait for connection object port
  }

  unsigned long timeout = 10000;
  if (waitForGSMModem(timeout) == true) {
    //gDeviceState->set(EDeviceState::modem, ON);
    //ladln("GSM modem is ready");

    if (!this->hwSBSIM800L.begin(*hardwareSerialBus)) { // Adafruit_FONA is starting now some standard routines on the GSM modem
      //ladln(F("Couldn't find\r\nGSM SIM800L"));
      // TODO ERROR
      while (1); // stay here
    }

    //ladln("Connecting GSM\r\nnetwork...");
    // modem is on, lets try to cinnect GSM network
    if (waitForNetwork(WaitForGSMNetWorkTimeout)) { // wait upto 30 seconds
      //gDeviceState->set(EDeviceState::network, ON);
      //ladln("GSM Network\r\nconnected.");
      //updateSignalStrengthIfNeeded();

      // network is available, so let's activate the network time sync, so that we later could show the current time
      if (this->hwSBSIM800L.enableNetworkTimeSync(true) == true) {
        Serial.println(F("Enabled GSM network time"));
        //gDeviceState->set(EDeviceState::networkTime, ON);
      } else {
        Serial.println(F("Failed to enable Network Time"));
        // TODO ERROR
      }
    } else {
      //ladln("GSM Network\r\nnot available.");
      // TODO ERROR
    }
  } else {
    // TODO ERROR
    //ladln("GSM modem can\r\nnot be activated!");
  }
}

bool GSMModem::setup() {

  

  return true; 
}



bool GSMModem::waitForGSMModem(unsigned long timeout = 10000) {
  unsigned long start = millis();
  String response = "";
  
  while (millis() - start < timeout) {
    hardwareSerialBus->println("AT");
    SerialIDE.println("... still WAIT");
    delay(100);
    
    while (hardwareSerialBus->available()) {
      // was hard to find a running solution to read on the hardware serial bus, because the indexOf function can not find the search string if you read char by char on bus
      // here are now 3 solutions they will work

      // 1. Read as String. (this reads until end of line was found)
      // String strResponse = gGSMModemBus->readString();
      // SerialIDE.println("STR Response: '" + strResponse + "'");
      // int okIndex = strResponse.indexOf("OK");

      // 2. read into a char array
      // char response[100] = {0}; // Initialisiert alle Elemente mit 0
      // int index = 0;
      // while (gGSMModemBus->available() && index < 99) {
      //   response[index++] = gGSMModemBus->read();
      // }
      // response[index] = '\0'; // Explizite Null-Terminierung

      // String checkResponseString = String(response);
      // int okIndex = checkResponseString.indexOf("OK");


      // 3. read char by char, but ignore Null terminators 
      char c = hardwareSerialBus->read();
      if (c != '\0') {
        response += c;
        
        int okIndex = response.indexOf("OK");  
        if (okIndex != -1 ){
          SerialIDE.println("Received '" + response + "' from GSM Modem.");
          return true; // SIM800L is ready
        } 
      }      
    }
    
    delay(900); // wait a second before next try
  }
  
  return false; // Timeout erreicht, Modem nicht bereit
}

bool GSMModem::isGSMModemOnline() {
  bool result = false;
  if (waitForGSMModem(100) == true) {
    // ok modem still is running
    Serial.println("Modem is running");
    result = true;
  } else {
    // oh what happens, modem is not answering, is it off ? 
    Serial.println("Modem is off, maybe to much power consuption?");
  }
  return result;
}


bool GSMModem::waitForNetwork(unsigned long timeout) {
  unsigned long start = millis();
  while (millis() - start < timeout) {
    if (isNetworkConnected() == true) {
      Serial.println("Mit Netzwerk verbunden!");
      return true;
    } else {
      Serial.println("Warte auf Netzwerkverbindung...");
      // was not able to connect network, maybe modem is offline ?
      if (isGSMModemOnline() == false) {
        // network connection is not possible, because modem is off
        break;
      }

      delay(1000); // wait some time, maybe model still is initalizing
    }
  }
  Serial.println("Netzwerkverbindung fehlgeschlagen");
  return false;
}


bool GSMModem::isNetworkConnected() {
  bool result = false;

  if (hwSBSIM800L.getNetworkStatus() == 1) {
    result = true;
  }
  return result;
}


int8_t GSMModem::getSignalStrengthDbm() {
  uint8_t rssi = hwSBSIM800L.getRSSI();
  int8_t dbm = -115;

  Serial.print(F("RSSI = ")); Serial.print(rssi); Serial.print(": ");

  // based on GSM standard 27007 (https://m10.home.xs4all.nl/mac/downloads/3GPP-27007-630.pdf). page 74
  if (rssi == 0) {
    dbm = -113;
  } else if (rssi == 1) {
    dbm = -111;
  } else if ((rssi >= 2) && (rssi <= 30)) {
    dbm = map(rssi, 2, 30, -109, -53);
  } else if (rssi = 31) {
    dbm = -51;
  }
  Serial.print(dbm); Serial.println(F(" dBm"));
  return dbm;
}


SignalStrength GSMModem::getSignalStrength() {
  int8_t dbm = getSignalStrengthDbm();

  // different meanings exists here: 
  // https://wiki.teltonika-gps.com/view/How_to_check_GSM_signal_strength?t
  // https://help.venntelecom.com/support/solutions/articles/44001930601-how-to-interpret-ec-io-sinr-rssi-rscp-rsrp-rsrq-parameters-in-2g-3g-lte-routers?t
  // https://help.venntelecom.com/support/solutions/articles/44001930999-how-to-interpret-rssi-parameters-in-2g-3g-and-lte-routers-?t

  // this is my way in the middle:

  SignalStrength strength = SignalStrength::zero; // <= -110

  if (dbm > -110 && dbm <= -95) {
    strength = SignalStrength::poor;
  } else if (dbm > -95 && dbm <= -80) {
    strength = SignalStrength::fair;
  } else if (dbm > -80 && dbm <= -65) {
    strength = SignalStrength::good;
  } else if (dbm > -65) {
    strength = SignalStrength::excellent;
  }

  //SignalStrength strength = (SignalStrength) map(dbm, -113, 51, 0, 4);
  return strength;
}


int8_t GSMModem::getCountSMS() {
  return this->hwSBSIM800L.getNumSMS();
}

bool GSMModem::readSMS(uint8_t messageIndex, char *smsbuff, uint16_t max, uint16_t *readsize) {
  return this->hwSBSIM800L.readSMS(messageIndex, smsbuff, max, readsize);
}

bool GSMModem::getTime(char* timeBuffer, uint16_t maxLength) {
  return this->hwSBSIM800L.getTime(timeBuffer, maxLength);
}
bool GSMModem::deleteSMS(uint8_t messageIndex) {
  return this->hwSBSIM800L.deleteSMS(messageIndex);
}


  

  // char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!
  // uint8_t imeiLen = sim800l.getIMEI(imei);
  // if (imeiLen > 0) {
  //   lad("SIM card IMEI: ", false); ladln(imei), false;
  // }

  // uint8_t netStatus = sim800l.getNetworkStatus();
  // if (netStatus > 0) {
  //   lad("Net Status: ", false); ladln(netStatus, false);
  // } else {
  //   ladln("Net Status: error"); 
  // }

  // uint8_t smsstorage = gGSMModemBus->print("AT+CPMS=?\r\n");
  // if (smsstorage > 0) {
  //   lad("Storage Status: ", false); ladln(smsstorage, false);
  // } else {
  //   ladln("Storage Status: error"); 
  // }

//sim800l.unlockSIM(simPIN);
//  delay(2000);
