#ifndef SLACK_H
#define SLACK_H


#include <Arduino.h>
#include <HTTPClient.h>
#include "globals.h"

class Slack {
  private:
    // Private Attribute
    String webHook;
    
  public:
    // Constructor
    Slack(String webHook);
    
    // public Methoden
    bool sendMessage(String message); 
};


#endif 