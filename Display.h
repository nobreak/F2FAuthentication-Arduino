#ifndef Display_h
#define Display_h

#include "globals.h"


#define WHITE SSD1306_WHITE
#define BLACK SSD1306_BLACK

void clearRectOnDisplay(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color = BLACK);
void updateIconSignalStrength(SignalStrength strength);
void updateTextWithCurrentTime(char* buffer);
void updateIconWifi(bool isVisible);

#endif