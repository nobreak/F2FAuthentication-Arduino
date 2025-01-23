#ifndef DeviceState_H
#define DeviceState_H

#define ON true
#define OFF false

#include "globals.h"


enum EDeviceState {
  modem = 0,
  network,
  networkTime,
  wifi,
  countStates
};


class DeviceState {

  private: 
    byte mDeviceState;

  public:
    // Constructor    
    DeviceState();
    void resetAll();
    void set(EDeviceState stateID, bool value);
    bool get(EDeviceState stateID);

};

#endif