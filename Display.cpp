

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

    clearRectOnDisplay(120, 0, 128, 8);
    gDisplay.drawBitmap(120,0, resultIcon, 8,8, WHITE);
    gDisplay.display();
    
}
