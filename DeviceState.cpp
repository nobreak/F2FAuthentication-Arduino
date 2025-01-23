#include "deviceState.h"




DeviceState::DeviceState() {
  this->resetAll();
}


void DeviceState::set(EDeviceState stateID, bool value) {
  if (value == true) {
    bitSet(this->mDeviceState, stateID);
  } else {
    bitClear(this->mDeviceState, stateID);
  }
}

bool DeviceState::get(EDeviceState stateID) {
  bool result = false;
  if (bitRead(this->mDeviceState, stateID)) {
    result = true;
  }
  return result; 
}



void DeviceState::resetAll() {
  for (int8_t s=0; s < EDeviceState::countStates; s++) {
    bitClear(this->mDeviceState, s);
  }
}

