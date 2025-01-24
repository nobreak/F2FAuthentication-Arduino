
#ifndef GSMModem_h
#define GSMModem_h


#include <Adafruit_FONA.h>
#include "globals.h"


// choose one of them
#define BAUDRATE_SMS_CALL 9600
#define BAUDRATE_GPRS 57600
#define BAUDRATE_IOT_FIRMWAREUPDATE 115200


enum SignalStrength {
  zero, 
  poor, 
  fair, 
  good, 
  excellent 
};

struct GSMModemInfo {
  int8_t pinTX = 27; 
  int8_t pinRX = 26; 
  int8_t pinRST = 5; 
  int8_t pinPwrKey = 4; 
  int8_t pinPower = 23;
  unsigned long serialBaudrate = BAUDRATE_SMS_CALL;
};


class GSMModem {
  public:
    GSMModem(HardwareSerial* hardwareSerialBus, GSMModemInfo modemInfo);

    int8_t getCountSMS();
    bool readSMS(uint8_t messageIndex, char *smsbuff, uint16_t max, uint16_t *readsize);
    bool getTime(char* timeBuffer, uint16_t maxLength);
    bool deleteSMS(uint8_t messageIndex);
    void reset();
    bool isNetworkConnected();
    SignalStrength getSignalStrength();
    uint8_t getIMEI(char *imei);
    uint8_t getSMSStorageStatus(); // beta
    uint8_t unlockSIM(char *pin); // beta


  private: 
    bool waitForGSMModem(unsigned long timeout);
    bool waitForNetwork(unsigned long timeout);
    int8_t getSignalStrengthDbm();
    bool isGSMModemOnline();

    HardwareSerial* hardwareSerialBus;
    GSMModemInfo info;
    Adafruit_FONA hwSBSIM800L; 
};

#endif 