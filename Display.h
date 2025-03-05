#ifndef Display_h
#define Display_h


#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>


#define WHITE SSD1306_WHITE
#define BLACK SSD1306_BLACK

class F2FADisplay : public Adafruit_SSD1306 {

  public:
    enum DisplaySignalStrength {
      zero, 
      poor, 
      fair, 
      good, 
      excellent 
    };

    bool isConnected = false;

    F2FADisplay(uint8_t w, uint8_t h, uint8_t i2caddr = 0x3C, uint8_t switchvcc = SSD1306_SWITCHCAPVCC);
    void clearRectOnDisplay(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color = BLACK);
    void updateIconSignalStrength(DisplaySignalStrength strength);
    void updateTextWithCurrentTime(char* buffer);
    void updateIconWifi(bool isVisible);
    void updateIconMessage(int8_t countSms);
};


#endif