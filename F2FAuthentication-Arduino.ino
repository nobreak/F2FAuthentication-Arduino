
#include "globals.h"
#include "GSMModem.h"
#include "Display.h"



/** 
* Serial bus setup for communication to IDE and SIM800L
**/
HardwareSerial *gGSMModemBus = &Serial1;
// Later we want to cast the gGSMModemBus object to Adafruit_FONA to have some nice GSM AT commands integrated in functions
Adafruit_FONA sim800l = Adafruit_FONA(SIM800L_PWRKEY); 



/**
* Display
**/
Adafruit_SSD1306 gDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


/**
* Constants maybe necessary
**/
char simPIN[]   = "1526"; // SIM card PIN code, if any

/**
* SPecial Runtime feature toggles
**/
// if there is the need to reset the modem
//#define RESET_SIM800 


/**
* Loop timer
**/
unsigned long gSignalStrengthPrevMillis = 0;
const long gSignalStrengthInterval = 5000; // Intervall von 5 Sekunden


void ladln(const String &text, bool toDisplay = true) {
  Serial.println(text);

  if (toDisplay) {
    clearRectOnDisplay(0,8,128,32);
    gDisplay.setCursor(0, 8);
    gDisplay.println(text);
    gDisplay.display();
  }
}

void lad(const String &text, bool toDisplay = true) {
  Serial.print(text);

  if (toDisplay) {
    clearRectOnDisplay(0,8,128,32);
    gDisplay.setCursor(0, 8);
    gDisplay.print(text);
    gDisplay.display();
  }
}

void ladln(int number, bool toDisplay = true) {
  Serial.println(number);

  if (toDisplay) {
    clearRectOnDisplay(0,8,128,32);
    gDisplay.setCursor(0, 8);
    gDisplay.println(number);
    gDisplay.display();
  }
}

void lad(int number, bool toDisplay = true) {
  Serial.print(number);

  if (toDisplay) {
    clearRectOnDisplay(0,8,128,32);
    gDisplay.setCursor(0, 8);
    gDisplay.print(number);
    gDisplay.display();
  }
}




void setup() {
  
  // set pins into right mode
  pinMode(LED_BLUE, OUTPUT); // we want to have the blue pin available
  pinMode(SIM800L_POWER, OUTPUT); // thats the pin to activate the SIM800

  // Keep reset PIN high, thats required in case you want to reset the sim800, you have to set to low
  pinMode(SIM800L_RST, OUTPUT);
  digitalWrite(SIM800L_RST, HIGH);

  // we want to see that ESP32 is on, so lets switch on the blue MC LED
  digitalWrite(LED_BLUE, HIGH);

  // you want reset the sim800?
  #ifdef RESET_SIM800
    digitalWrite(SIM800L_RST, LOW);
    delay(100);
    digitalWrite(SIM800L_RST, HIGH); 
    delay(3000); // wait for restart
  #endif
  
  //Begin serial communication with ESP32 and Arduino IDE (Serial Monitor to log into IDE what happens)
  SerialIDE.begin(115200);
  // We dont want to loose any messages after first boot of the device, 
  // so we wait for the object and additionally some time (5s)
  delay(5000);
  while (!SerialIDE) {
    ; // wait for connection object port
  }

  // Display
  if (!gDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Adresse prüfen
    Serial.println(F("Display konnte nicht initialisiert werden"));
    for (;;);
  }

  gDisplay.clearDisplay();
  gDisplay.setTextSize(1);
  gDisplay.setTextColor(SSD1306_WHITE);
  gDisplay.setRotation(0);
  gDisplay.invertDisplay(false);
  

  updateIconSignalStrength(SignalStrength::zero);
  
  ladln(F("#### F2FA on ESP32 with GSM SIM800L ####"), false);
  ladln(F("Initalising GSM modem ..."));


  // now lets switch on the SIM800 modem
  digitalWrite(SIM800L_POWER, HIGH);

//Begin serial communication with Arduino and SIM800L
  gGSMModemBus->begin(BAUDRATE_SMS_CALL);
  while (!gGSMModemBus) {
    ; // wait for connection object port
  }
  
  ladln("Wait for GSM modem...", false);
  unsigned long timeout = 10000;
  if (waitForGSMModem(timeout)) {
    ladln("GSM modem is ready");
  } else {
    ladln("GSM modem can not activated!");
  }
  
  if (!sim800l.begin(*gGSMModemBus)) {
    ladln(F("Couldn't find GSM SIM800L"));
    while (1); // stay here
  }

  ladln("Connecting network ...");
  if (waitForNetwork(60000)) { // Warte bis zu 60 Sekunden
    ladln("Network connected.");
    getSignalStrength();
  } else {
    ladln("Network not connectable.");
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

  // Set up the FONA to send a +CMTI notification
  // when an SMS is received
  /*gGSMModemBus->print("AT+CNETLIGHT=1\r\n"); 

  gGSMModemBus->print("AT+CNMI=2,1\r\n");

  gGSMModemBus->print("AT\r\n");
  */

  //ladln(F("GSM modem is ready!"));
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


long prevMillis = 0;
int interval = 1000;
char sim800lNotificationBuffer[64];          //for notifications from the FONA
char smsBuffer[250];
String smsString = "";

void loop() {

  
  unsigned long currentMillis = millis();

  // reglular check for signal strength
  if (currentMillis - gSignalStrengthPrevMillis >= gSignalStrengthInterval) {
    gSignalStrengthPrevMillis = currentMillis;
    SignalStrength sgnStrngth = getSignalStrength();
    Serial.print(sgnStrngth); Serial.println(F(" Signal Strength"));
    updateIconSignalStrength(sgnStrngth);
  }
    
    

  // put your main code here, to run repeatedly:


  char* bufPtr = sim800lNotificationBuffer;    //handy buffer pointer

  if (sim800l.available()) {
    int slot = 0; // this will be the slot number of the SMS
    int charCount = 0;

    // Read the notification into fonaInBuffer
    do {
      *bufPtr = sim800l.read();
      SerialIDE.write(*bufPtr);
      delay(1);
    } while ((*bufPtr++ != '\n') && (sim800l.available()) && (++charCount < (sizeof(sim800lNotificationBuffer)-1)));
    
    //Add a terminal NULL to the notification string
    *bufPtr = 0;

    //Scan the notification string for an SMS received notification.
    //  If it's an SMS message, we'll get the slot number in 'slot'
    if (1 == sscanf(sim800lNotificationBuffer, "+CMTI: \"SM\",%d", &slot)) {
      SerialIDE.print("slot: "); SerialIDE.println(slot);
      
      char callerIDbuffer[32];  //we'll store the SMS sender number in here
      
      // Retrieve SMS sender address/phone number.
      if (!sim800l.getSMSSender(slot, callerIDbuffer, 31)) {
        SerialIDE.println("Didn't find SMS message in slot!");
      }
      SerialIDE.print(F("FROM: ")); SerialIDE.println(callerIDbuffer);

      // Retrieve SMS value.
      uint16_t smslen;
      // Pass in buffer and max len!
      if (sim800l.readSMS(slot, smsBuffer, 250, &smslen)) {
        smsString = String(smsBuffer);
        SerialIDE.println(smsString);
      }

      if (smsString == "RELAY ON") {
        SerialIDE.println("Relay is activated.");
        delay(100);
        // Send SMS for status
        // if (!sim800l.sendSMS(callerIDbuffer, "Relay is activated.")) {
        //   SerialIDE.println(F("Failed"));
        // } else {
        //   SerialIDE.println(F("Sent!"));
        // }
      }
      else if (smsString == "RELAY OFF") {
        SerialIDE.println("Relay is deactivated.");
        // digitalWrite(RELAY, LOW);
        // delay(100);
        // // Send SMS for status
        // if (!sim800l.sendSMS(callerIDbuffer, "Relay is deactivated.")) {
        //   SerialIDE.println(F("Failed"));
        // } else {
        //   SerialIDE.println(F("Sent!"));
        // }
      }

      if (sim800l.deleteSMS(slot)) {
        SerialIDE.println(F("OK!"));
      }
      else {
        SerialIDE.print(F("Couldn't delete SMS in slot ")); SerialIDE.println(slot);
        sim800l.print(F("AT+CMGD=?\r\n"));
      }
    }


    forwardFromUart();
  
  }

}


