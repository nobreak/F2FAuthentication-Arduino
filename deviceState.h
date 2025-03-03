#ifndef DeviceState_H
#define DeviceState_H

#define ON true
#define OFF false

#include "globals.h"


enum EDeviceState {
  display = 0,
  modem,
  network,
  networkTime,
  wifi,
  countStates
};


class DeviceState {

  public:
    // Constructor    
    DeviceState();
    void resetAll();
    void set(EDeviceState stateID, bool value);
    bool get(EDeviceState stateID);
    void setSignalStrength(SignalStrength sgnStrength);
    SignalStrength getSignalStrength();
    void setLastNetworkTime(String networkTime);
    String getLastNetworkTime();
    void setCountNotForwardedSMS(uint8_t count);
    uint8_t getCountNotForwardedSMS();


    String getDescription(); 
    bool addErrorMessage(String errMsg);
    bool deleteAllErrorMessages(); 

  private: 
    String escapedTimeString(const String& input);
    bool containsErrorMessage(String value);


    byte mDeviceState;
    String* errorMessages = NULL;
    uint8_t currentCountErrorMessages = 0;
    SignalStrength mSignalStrength;
    String mLastNetworkTime;
    uint8_t mCountNotForwardedSMS = 0; // how much SMS we have on SIM chip which are not forwarded to slack 

};

#endif