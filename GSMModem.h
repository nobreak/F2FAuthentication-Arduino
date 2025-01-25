
#ifndef GSMModem_h
#define GSMModem_h


#include <Adafruit_FONA.h>


// choose one of them
#define BAUDRATE_SMS_CALL 9600
#define BAUDRATE_GPRS 57600
#define BAUDRATE_IOT_FIRMWAREUPDATE 115200

#define WaitForGSMNetWorkTimeout 30000
#define WaitforGSMModemTimeout 10000



enum EGSMModemState : uint8_t {
  initializing = 0,
  modemOnline,
  networkConnected,
  networkTimeEnabled,
  countModemStates
};

enum EGSMModemError : uint8_t {
  ErrorSerialBusSim800L = 10,
  ErrorNetworkTime,
  ErrorNetworkConnection,
  ErrorGSMModem
};

enum EGSMErrorType : uint8_t {
  Critical = 1,
  Normal,
  Warning
};


class GSMModemDelegate {
public:
    virtual void onModemStatusChanged(EGSMModemState changedState, bool newState) = 0;
    virtual void onModemError(EGSMModemError errorCode, EGSMErrorType errorType, char* description) = 0;
    virtual ~GSMModemDelegate() {}  // Virtual Destructor
};


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
    GSMModem(HardwareSerial* hardwareSerialBus, GSMModemInfo modemInfo, GSMModemDelegate* delegate = NULL, char* simPIN = NULL);

    void setup();
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
    bool getState(EGSMModemState state);

    void setDelegate(GSMModemDelegate* delegate);


  private: 
    bool waitForGSMModem(unsigned long timeout);
    bool waitForNetwork(unsigned long timeout);
    int8_t getSignalStrengthDbm();
    bool isGSMModemOnline();
    void setState(EGSMModemState state, bool value);
    void resetAllStates();

    HardwareSerial* hardwareSerialBus = NULL;
    GSMModemInfo info;
    Adafruit_FONA hwSBSIM800L;
    char* simPIN = NULL;  
    byte mState; // modem state bitmask
    GSMModemDelegate* mDelegate = NULL;

};

#endif 