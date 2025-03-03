#include <Arduino.h>
#include "GSMModem.h"

#define SerialIDE Serial0
#define SENDEVENT(...) \
    if (mDelegate) { \
        mDelegate->onModemStatusChanged(__VA_ARGS__); \
    }

#define HANDLEERROR(...) \
  if (mDelegate) { \
    mDelegate->onModemError(__VA_ARGS__); \
  }

GSMModem::GSMModem(HardwareSerial* hardwareSerialBus, GSMModemInfo modemInfo, GSMModemDelegate* delegate, char* simPIN) : hwSBSIM800L(modemInfo.pinPwrKey) {
  this->resetAllStates();
  this->hardwareSerialBus = hardwareSerialBus;
  this->info = modemInfo;
  this->setDelegate(delegate);
  this->simPIN = simPIN;
}

void GSMModem::setup() {
  setState(initializing, true);

  // thats the pin to activate the SIM800
  pinMode(info.pinPower, OUTPUT); 
  // Keep reset PIN high, thats required in case you want to reset the sim800, you have to set to low
  pinMode(info.pinRST, OUTPUT);
  digitalWrite(info.pinRST, HIGH);

  // now lets switch on the SIM800 modem
  digitalWrite(info.pinPower, HIGH);

  //Begin serial communication with Arduino and SIM800L
  hardwareSerialBus->begin(info.serialBaudrate, SERIAL_8N1, info.pinRX, info.pinTX);
  while (!hardwareSerialBus) {
    ; // wait for connection object port
  }

  if (waitForGSMModem(WaitforGSMModemTimeout) == true) {
    setState(modemOnline, true);

    if (!this->hwSBSIM800L.begin(*hardwareSerialBus)) { // Adafruit_FONA is starting now some standard routines on the GSM modem
      HANDLEERROR(ErrorSerialBusSim800L, Critical, "CRITICAL: FONA can not begin serial bus communication!" )
      while (1); // stay here
    }

    // modem is on, lets try to connect GSM network
    if (waitForNetwork(WaitForGSMNetWorkTimeout)) { // wait upto 30 seconds
      setState(networkConnected, true);

      // network is available, so let's activate the network time sync, so that we later could show the current time
      if (this->hwSBSIM800L.enableNetworkTimeSync(true) == true) {
        setState(networkTimeEnabled, true);
      } else {
        HANDLEERROR(ErrorNetworkTime, Warning, "WARNING: Networktime can not be enabled!" )
      }
    } else {
      HANDLEERROR(ErrorNetworkConnection, Normal, "GSM Network not reachable, device is trying to restart..." )
    }
  } else {
    HANDLEERROR(ErrorGSMModem, Critical, "CRITICAL: GSM modem can not be activated!" )
    while (1); // stay here
  }

  setState(initializing, false);
}


void GSMModem::setDelegate(GSMModemDelegate* delegate) {
  this->mDelegate = delegate;
  SerialIDE.println("delegate was set");
}


void GSMModem::setState(EGSMModemState state, bool value) {
  bool oldValue = getState(state);

  if (value == true) {
    bitSet(this->mState, state);
  } else {
    bitClear(this->mState, state);
  }
  
  if (mDelegate) {
    if (oldValue != value) {
      mDelegate->onModemStatusChanged(state, value); 
    }
  } else {
    // TODO ERROR
    SerialIDE.println("ERROR: no delegate!");
  }
  //SENDEVENT(state, value)
}

bool GSMModem::getState(EGSMModemState state) {
  bool result = false;
  if (bitRead(this->mState, state)) {
    result = true;
  }
  return result; 
}

void GSMModem::resetAllStates() {
  for (int8_t s=0; s < countModemStates; s++) {
    bitClear(this->mState, s);
  }
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
    // todo error handling and state handling
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
         // todo error handling and state handling
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

void GSMModem::reset() {
  digitalWrite(info.pinRST, LOW);
  delay(100);
  digitalWrite(info.pinRST, HIGH); 
  delay(3000); // wait for restart
}

// returns the length of the IMEI, should be 16
uint8_t GSMModem::getIMEI(char *imei) {
  return this->hwSBSIM800L.getIMEI(imei);
}

// beta
uint8_t GSMModem::getSMSStorageStatus() {
  uint8_t smsStorageStatus = hardwareSerialBus->print("AT+CPMS=?\r\n");
  return smsStorageStatus;
  // if (smsStorageStatus > 0) {
  //   Serial.print("Storage Status: ", false); Serial.println(smsstorage, false);
  // } else {
  //   Serial.println("Storage Status: error"); 
  // }
}

// beta, should we have an delay ?
uint8_t GSMModem::unlockSIM(char *pin) {
    return this->hwSBSIM800L.unlockSIM(pin);
  //  delay(2000);
}

