#include "slack.h"


Slack::Slack(String webHook) {
  this->webHook = webHook;
}

bool Slack::sendMessage(String message) {
  bool result = false;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Slack Webhook URL
    http.begin(SLACK_WEBHOOK_URL);
    http.addHeader("Content-Type", "application/json");
    
    // JSON-Payload erstellen
    String payload = "{\"text\":\"" + message + "\"}";
    
    Serial.println("Slack PAYLOAD:");
    Serial.println(payload);

    // POST-Anfrage senden
    int httpResponseCode = http.POST(payload);
    
    if (httpResponseCode > 0 ) {
      String response = http.getString();
      if (httpResponseCode >= 300) {
          Serial.println("Error for HTTP POST: " + String(httpResponseCode));
      } else {
          Serial.println("HTTP response code: " + String(httpResponseCode));
          result = true;
      }
      Serial.println("Response: " + response);
    } else {
      Serial.println("Unknown error during HTTP POST: " + String(httpResponseCode));
    }
    
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }

  return result;
}

bool Slack::sendDeviceStatus() {

  // create the status text:
  // Current Device State
  return true;
}

