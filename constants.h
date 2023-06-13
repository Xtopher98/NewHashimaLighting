#ifndef CONSTANTS
#define CONSTANTS

//pinout of led driver board
#define SIGN_TOP 1
#define SIGN_BOTTOM 2
#define DOT_RIGHT 6
#define DOT_LEFT 7
#define ARCH_RIGHT 12
#define ARCH_LEFT 13
#define CORNER_RIGHT 14
#define CORNER_LEFT 15

#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 255
#define LOW  MIN_BRIGHTNESS
#define HIGH MAX_BRIGHTNESS

uint8_t leds[] = { DOT_RIGHT, DOT_LEFT, ARCH_RIGHT, ARCH_LEFT, CORNER_RIGHT, CORNER_LEFT, SIGN_TOP, SIGN_BOTTOM };
#define NUM_LEDS (sizeof(leds) / sizeof(leds[0]))

#define GAMMA 1.5 //for perceptually-linear brightness
//bitmaps for 8x8 Matrix
static const uint8_t PROGMEM
  smile_bmp[] =
  { 0b00111100,
    0b01000010,
    0b10100101,
    0b10000001,
    0b10100101,
    0b10011001,
    0b01000010,
    0b00111100 },
  neutral_bmp[] =
  { 0b00111100,
    0b01000010,
    0b10100101,
    0b10000001,
    0b10111101,
    0b10000001,
    0b01000010,
    0b00111100 },
  frown_bmp[] =
  { 0b00111100,
    0b01000010,
    0b10100101,
    0b10000001,
    0b10011001,
    0b10100101,
    0b01000010,
    0b00111100 },
  
  //arrow pointing left
  left_bmp[]
  { 0b00010000,
    0b00110000,
    0b01110000,
    0b11111111,
    0b11111111,
    0b01110000,
    0b00110000,
    0b00010000 };

#endif