
#include "GSMModem.h"



bool waitForGSMModem(unsigned long timeout = 10000) {
  unsigned long start = millis();
  String response = "";
  
  while (millis() - start < timeout) {
    gGSMModemBus->println("AT");
    SerialIDE.println("... still WAIT");
    delay(100);
    
    while (gGSMModemBus->available()) {
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
      char c = gGSMModemBus->read();
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

bool isGSMModemOnline() {
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


bool waitForNetwork(unsigned long timeout) {
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


bool isNetworkConnected() {
  bool result = false;

  if (sim800l.getNetworkStatus() == 1) {
    result = true;
  }
  return result;
}


int8_t getSignalStrengthDbm() {
  uint8_t rssi = sim800l.getRSSI();
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


SignalStrength getSignalStrength() {
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
