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

String DeviceState::getDescription() {
  String result = "";

  for (int8_t s=0; s < EDeviceState::countStates; s++) {
    switch (s) {
      case EDeviceState::display: 
        result += "Display: ";
        if (get(EDeviceState::display) == ON) {
          result += "ON";
        } else {
          result += "OFF";
        }  
        result += "\\r\\n";
        break;
      case EDeviceState::modem:
        result += "GSM Modem: ";
        if (get(EDeviceState::modem) == ON) {
          result += "ON";
        } else {
          result += "OFF";
        }  
        result += "\\r\\n";
        break;
      case EDeviceState::network:
        result += "GSM Network: ";
        if (get(EDeviceState::network) == ON) {
          result += "Connected";
        } else {
          result += "NOT Connected";
        }  
        result += "\\r\\n";
        break;
      case EDeviceState::networkTime:
        result += "GSM Network Time: ";
        if (get(EDeviceState::networkTime) == ON) {
          result += "Enabled";
        } else {
          result += "NOT Enabled";
        }  
        result += "\\r\\n";
        break;
      case EDeviceState::wifi:
        result += "Wifi: ";
        if (get(EDeviceState::wifi) == ON) {
          result += "Connected";
        } else {
          result += "NOT Connected";
        }  
        result += "\\r\\n";
        break;
      default: {
        result += "Unknown state: ";
        if (get((EDeviceState)s) == ON) {
          result += "ON";
        } else {
          result += "OFF";
        }  
        result += "\\r\\n";
      }
    } // switch
  } // for loop

  return result;
}

