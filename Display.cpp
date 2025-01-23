

#include "Display.h"
#include "icons.h"



F2FADisplay::F2FADisplay(uint8_t w, uint8_t h, uint8_t i2caddr, uint8_t switchvcc) : Adafruit_SSD1306(w,h,&Wire,-1) {

  if (!begin(switchvcc, i2caddr)) { 
    Serial.println(F("Error: Display\r\nis not initalized"));
  } else {
    isConnected = true;
    clearDisplay();
    setTextSize(1);
    setTextColor(WHITE);
    setRotation(0);
    invertDisplay(false);
    updateIconSignalStrength(F2FADisplay::DisplaySignalStrength::zero);
  }
}


// You have to call gDisplay.display(); after 
void F2FADisplay::clearRectOnDisplay(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  this->fillRect(x,y,w,h,color);
}

void F2FADisplay::updateIconSignalStrength(DisplaySignalStrength strength) {

  const unsigned char* resultIcon = iconX;

    switch (strength) {
      case DisplaySignalStrength::poor:  
        resultIcon = iconSgnStrengthPoor;
        break;
      case DisplaySignalStrength::fair:  
        resultIcon = iconSgnStrengthFair;
        break;
      case DisplaySignalStrength::good:
        resultIcon = iconSgnStrengthGood;
        break;
      case DisplaySignalStrength::excellent:
        resultIcon = iconSgnStrengthExcellent;
        break;      
    }

    clearRectOnDisplay(120, 0, 8, 8);
    drawBitmap(120,0, resultIcon, 8,8, WHITE);
    display();
}

void F2FADisplay::updateIconWifi(bool isVisible){
  clearRectOnDisplay(110, 0, 8, 8);
  if (isVisible == true) {
    const unsigned char* resultIcon = iconWifi;
    drawBitmap(110,0, resultIcon, 8,8, WHITE);
  } 
  display();
}


void F2FADisplay::updateIconMessage(int8_t countSms) {
  clearRectOnDisplay(0, 0, 32, 8);
  
  if (countSms > 0 ) {
    const unsigned char* resultIcon = iconMessage;
    drawBitmap(0,0, resultIcon, 12,8, WHITE);
    setCursor(14, 0);
    print(countSms);
  }
  
  display();
}


// is avainting a charbuffer with this formated text inside: 25/01/20,02:19:12+04
void F2FADisplay::updateTextWithCurrentTime(char* buffer){
    int startIndex = 10;
    int length = 5; // count if visible digits

    clearRectOnDisplay(0,8,128,24);
    setCursor(36,12); 
    setTextSize(2);
    write(buffer + startIndex, length);
    display();
    setTextSize(1);
}
