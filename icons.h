#ifndef icons_h
#define icons_h

const unsigned char PROGMEM heartIcon[] = {
  B00110110,
  B01111111,
  B01111111,
  B01111111,
  B00111110,
  B00011100,
  B00001000
};


const unsigned char PROGMEM iconSgnStrengthPoor[] = {
  0x0,  
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  B01000000,
  B01000000
};

const unsigned char PROGMEM iconSgnStrengthFair[] = {
  0x0,  
  0x0,
  0x0,
  0x0,
  B00010000,
  B00010000,
  B01010000,
  B01010000
};

const unsigned char PROGMEM iconSgnStrengthGood[] = {
  0x0,  
  0x0,
  B00000100,
  B00000100,
  B00010100,
  B00010100,
  B01010100,
  B01010100
};

const unsigned char PROGMEM iconSgnStrengthExcellent[] = {
  B00000001,  
  B00000001,
  B00000101,
  B00000101,
  B00010101,
  B00010101,
  B01010101,
  B01010101
};

const unsigned char PROGMEM iconX[] = {
  B00000000,  
  B01000010,
  B00100100,
  B00011000,
  B00011000,
  B00100100,
  B01000010,
  B00000000
};

#endif