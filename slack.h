#ifndef SLACK_H
#define SLACK_H


#include <Arduino.h>
#include <HTTPClient.h>
#include "globals.h"

class Slack {
  private:
    String escapedString(const String& input);
    String webHook;
    
  public:
    // Constructor
    Slack(String webHook);
    
    // public Methoden
    bool sendMessage(String message, bool shouldEscapeStrings = true); 
};


#endif 