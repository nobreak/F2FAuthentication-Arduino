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


// 8x8
// const unsigned char PROGMEM iconMessage[] = {
//   B11111111,
//   B10000001,
//   B11000011,
//   B10100101,
//   B10011001,
//   B10100101,
//   B11000011,
//   B11111111
// };

// 16x8
// const PROGMEM uint8_t iconMessage[] = {
//   0b11111111, 0b11111111,
//   0b10000000, 0b00000001,
//   0b10100000, 0b00000101,
//   0b10010000, 0b00001001,
//   0b10001000, 0b00010001,
//   0b10000100, 0b00100001,
//   0b10000010, 0b01000001,
//   0b11111111, 0b11111111
// };

// 12x8
const PROGMEM uint8_t iconMessage[] = {
  0b11111111, 0b11110000,
  0b10000000, 0b00010000,
  0b10100000, 0b01010000,
  0b10010001, 0b00010000,
  0b10001010, 0b00010000,
  0b10000100, 0b00010000,
  0b10000000, 0b00010000,
  0b11111111, 0b11110000
};


const unsigned char PROGMEM iconWifi[] = {
  B00000000,
  B00011000,
  B01100110,
  B10000001,
  B00111100,
  B01000010,
  B00011000,
  B00011000,
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