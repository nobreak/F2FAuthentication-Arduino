

#include "Display.h"
#include "icons.h"




// You have to call gDisplay.display(); after 
void clearRectOnDisplay(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  gDisplay.fillRect(x,y,w,h,color);
}

void updateIconSignalStrength(SignalStrength strength) {

  const unsigned char* resultIcon = iconX;

    switch (strength) {
      case SignalStrength::poor:  
        resultIcon = iconSgnStrengthPoor;
        break;
      case SignalStrength::fair:  
        resultIcon = iconSgnStrengthFair;
        break;
      case SignalStrength::good:
        resultIcon = iconSgnStrengthGood;
        break;
      case SignalStrength::excellent:
        resultIcon = iconSgnStrengthExcellent;
        break;      
    }

    clearRectOnDisplay(120, 0, 8, 8);
    gDisplay.drawBitmap(120,0, resultIcon, 8,8, WHITE);
    gDisplay.display();
}

void updateIconWifi(bool isVisible){
  
  clearRectOnDisplay(111, 0, 8, 8);

  if (isVisible == true) {
    const unsigned char* resultIcon = iconWifi;
    gDisplay.drawBitmap(111,0, resultIcon, 8,8, WHITE);
  } 

  gDisplay.display();
  
}

// is avainting a charbuffer with this formated text inside: 25/01/20,02:19:12+04
void updateTextWithCurrentTime(char* buffer){
    int startIndex = 10;
    int length = 5; // count if visible digits

    clearRectOnDisplay(0,8,128,24);
    gDisplay.setCursor(36,12); 
    gDisplay.setTextSize(2);
    gDisplay.write(buffer + startIndex, length);
    gDisplay.display();
    gDisplay.setTextSize(1);
}
