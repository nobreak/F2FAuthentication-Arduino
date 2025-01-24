#include "slack.h"


Slack::Slack(String webHook) {
  this->webHook = webHook;
}

bool Slack::sendMessage(String message, bool shouldEscapeStrings) {
  bool result = false;

  if (shouldEscapeStrings == true) {
    Serial.print("BEFORE:"); Serial.println(message);
    message = escapedString(message);
    Serial.print("After:"); Serial.println(message);
  }

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


String Slack::escapedString(const String& input) {
  String output;
  for (unsigned int i = 0; i < input.length(); i++) {
    char c = input.charAt(i);
    switch (c) {
      case '\\': output += "\\\\"; break;
      case '\"': output += "\\\""; break;
      //case '\n': output += "\\n"; break;
      //case '\r': output += "\\r"; break;
      //case '\t': output += "\\t"; break;
      default: output += c;
    }
  }
  return output;
}


