#include "deviceState.h"

#define MAX_ERROR_MESSAGES 20


DeviceState::DeviceState() {
  this->resetAll();
  this->errorMessages = new String[MAX_ERROR_MESSAGES];
}


bool DeviceState::addErrorMessage(String errMsg) {
  bool result = false;
  
  if (errorMessages == NULL) {
    this->errorMessages = new String[MAX_ERROR_MESSAGES];
  }

  if (currentCountErrorMessages < MAX_ERROR_MESSAGES ) {
    errorMessages[currentCountErrorMessages] = errMsg;
    currentCountErrorMessages++;
    result = true;
  }
  return result;
}

bool DeviceState::deleteAllErrorMessages() {
  bool result = false;
  if (errorMessages != NULL) {
    delete[] errorMessages;
    errorMessages = NULL;
    currentCountErrorMessages = 0;
    result = true;
  }
  return result;
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

  if (errorMessages != NULL) {
    result += "Errors:\\r\\n";
    for (int e = 0; e < currentCountErrorMessages; e++) {
      result += "  - ";
      result += errorMessages[e];
  
      // add line break
      result += "\\r\\n";
    } // for loop
  } // errMSg condition

  return result;
}

