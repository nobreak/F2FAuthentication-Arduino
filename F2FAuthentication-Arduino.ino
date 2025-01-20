
#include "globals.h"
#include "GSMModem.h"
#include "Display.h"
#include "environment_secrets.h"

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;



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
char simPIN[]   = SIM_PIN; // SIM card PIN code, if any

/**
* SPecial Runtime feature toggles
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


bool sendSlackMessage(String message) {
  bool result = false;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Slack Webhook URL
    http.begin(SLACK_WEBHOOK_URL);
    http.addHeader("Content-Type", "application/json");
    
    // JSON-Payload erstellen
    String payload = "{\"text\":\"" + message + "\"}";
    
    // POST-Anfrage senden
    int httpResponseCode = http.POST(payload);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Antwort-Code: " + String(httpResponseCode));
      Serial.println("Antwort: " + response);
      result = true;
    } else {
      Serial.println("Fehler beim HTTP POST: " + String(httpResponseCode));
    }
    
    http.end();
  } else {
    Serial.println("WiFi nicht verbunden");
  }

  return result;
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
  if (!gDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // check the adresse with your device
    Serial.println(F("Error: Display is not initalized"));
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
  
  if (!sim800l.begin(*gGSMModemBus)) { // Adafruit_FONA is tarting now some standard routines on the GSM modem
    ladln(F("Couldn't find GSM SIM800L"));
    while (1); // stay here
  }

  ladln("Connecting GSM network ...");
  if (waitForNetwork(60000)) { // wait upto 60 seconds
    ladln("GSM Network connected.");
    updateSignalStrengthIfNeeded();
  } else {
    ladln("GSM Network not available.");
  }

  if (!sim800l.enableNetworkTimeSync(true)) {
    Serial.println(F("Failed to enable Network Time"));
  }

  // connecting Wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    ladln("Verbinde mit WLAN...");
  }
  ladln("Verbunden mit WLAN");




  // Set up the FONA to send a +CMTI notification
  // when an SMS is received
  /*gGSMModemBus->print("AT+CNETLIGHT=1\r\n"); 

  gGSMModemBus->print("AT+CNMI=2,1\r\n");

  gGSMModemBus->print("AT\r\n");
  */

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
  SignalStrength sgnStrngth = getSignalStrength();

  if (sgnStrngth != gLastSignalStrength) {
    Serial.print(F("New Signal Strength")); Serial.println(sgnStrngth);
    updateIconSignalStrength(sgnStrngth);
    gLastSignalStrength = sgnStrngth;
  }
}

void updateCurrentTime(){
    char timeBuffer[23];
    sim800l.getTime(timeBuffer, 23);  // getting the time via FONA from GSM modem
    Serial.print(F("Time = ")); Serial.println(timeBuffer);

    // if modem is not fully initalized, we're getting not valid date starting at 04/01/01, we ignore this
    const char* searchString = "04/01/01";
    char* result = strstr(timeBuffer, searchString);

    if (result != NULL) {
          Serial.print(F("Time = ")); Serial.print(timeBuffer);  Serial.print(F("++++NOT VALID"));
      return;
    }

    updateTextWithCurrentTime(timeBuffer);
}


void forwardAndDeleteSMSIfNeeded() {
  // get count of SMS
  int8_t countSms = sim800l.getNumSMS();
  if (countSms > 0) {
      Serial.print(F("Found ")); Serial.print(countSms); Serial.println(F(" new SMS's on SIM card!"));

      // open each SMS 
      uint16_t smslen;
      int8_t smsn = 1;
      char smsTextBuffer[255];

      for ( ; smsn <= countSms; smsn++) {
        Serial.print(F("\n\rReading SMS #")); Serial.println(smsn);
        if (!sim800l.readSMS(smsn, smsTextBuffer, 250, &smslen)) {  // pass in buffer and max len!
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

        String slackMessange = "Received New SMS: '" + String(smsTextBuffer) + "'"; 
        if (sendSlackMessage(String(slackMessange))) {
          // delete SMS
          Serial.print(F("\n\rDeleting SMS #")); Serial.println(smsn);
          if (sim800l.deleteSMS(smsn)) {
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

long prevMillis = 0;
int interval = 1000;
char sim800lNotificationBuffer[64];          //for notifications from the FONA
char smsBuffer[250];
String smsString = "";




/**
* Main Loop
**/
void loop() {
    unsigned long currentMillis = millis();

  // get the signal strength and update the diplay if it has changed
  if (currentMillis - gSignalStrengthPrevMillis >= gSignalStrengthInterval) {
    gSignalStrengthPrevMillis = currentMillis;
    updateSignalStrengthIfNeeded();

    //sendSlackMessage("Message from F2FA phone managed with secrets");
  }
    
  // get the time and update the display
  if (currentMillis - gTimePrevMillis >= gTimeInterval) {
    gTimePrevMillis = currentMillis;
    updateCurrentTime();
  }

  // get the count of received SMS and forward them
  // after it we delete the SMS

  if (currentMillis - gSMSPollPrevMillis >= gSMSPollInterval) {
    gSMSPollPrevMillis = currentMillis;
    forwardAndDeleteSMSIfNeeded();
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


