
#ifndef GSMModem_h
#define GSMModem_h

#include "globals.h"


enum SignalStrength {
  zero, 
  poor, 
  fair, 
  good, 
  excellent 
};

bool waitForGSMModem(unsigned long timeout);
bool waitForNetwork(unsigned long timeout);
bool isNetworkConnected();
SignalStrength getSignalStrength();
int8_t getSignalStrengthDbm();
bool isGSMModemOnline();



#endif 