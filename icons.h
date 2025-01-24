#ifndef icons_h
#define icons_h

const unsigned char PROGMEM heartIcon[] = {
  0b00110110,
  0b01111111,
  0b01111111,
  0b01111111,
  0b00111110,
  0b00011100,
  0b00001000
};


const unsigned char PROGMEM iconSgnStrengthPoor[] = {
  0x0,  
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0b01000000,
  0b01000000
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
  0b00000000,
  0b00011000,
  0b01100110,
  0b10000001,
  0b00111100,
  0b01000010,
  0b00011000,
  0b00011000,
};



const unsigned char PROGMEM iconSgnStrengthFair[] = {
  0x0,  
  0x0,
  0x0,
  0x0,
  0b00010000,
  0b00010000,
  0b01010000,
  0b01010000
};

const unsigned char PROGMEM iconSgnStrengthGood[] = {
  0x0,  
  0x0,
  0b00000100,
  0b00000100,
  0b00010100,
  0b00010100,
  0b01010100,
  0b01010100
};

const unsigned char PROGMEM iconSgnStrengthExcellent[] = {
  0b00000001,  
  0b00000001,
  0b00000101,
  0b00000101,
  0b00010101,
  0b00010101,
  0b01010101,
  0b01010101
};

const unsigned char PROGMEM iconX[] = {
  0b00000000,  
  0b01000010,
  0b00100100,
  0b00011000,
  0b00011000,
  0b00100100,
  0b01000010,
  0b00000000
};

#endif