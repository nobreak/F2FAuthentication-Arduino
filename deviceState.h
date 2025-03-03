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
  
    String getDescription(); 
    bool addErrorMessage(String errMsg);
    bool deleteAllErrorMessages(); 

  private: 
    byte mDeviceState;
    String* errorMessages = NULL;
    uint8_t currentCountErrorMessages = 0;
    SignalStrength mSignalStrength;

};

#endif