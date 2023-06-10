#ifndef CONSTANTS
#define CONSTANTS

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
#define LOW  0 //MIN_BRIGHTNESS
#define HIGH 255 //MAX_BRIGHTNESS

uint8_t leds[] = { DOT_RIGHT, DOT_LEFT, ARCH_RIGHT, ARCH_LEFT, CORNER_RIGHT, CORNER_LEFT };
uint8_t sign[] = { SIGN_TOP, SIGN_BOTTOM };
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
    0b00111100 };

#endif