#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AW9523.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_NeoPixel.h>
#include "constants.h"

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

uint8_t leds[] = { DOT_RIGHT, DOT_LEFT, ARCH_RIGHT, ARCH_LEFT, CORNER_RIGHT, CORNER_LEFT };
uint8_t sign[] = { SIGN_TOP, SIGN_BOTTOM };
#define NUM_LEDS (sizeof(leds) / sizeof(leds[0]))

#define GAMMA 2.6 //for perceptually-linear brightness

Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Adafruit_AW9523 ledDriver;
Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

uint32_t loopCounter = 0; //max value 2,147,483,647
#define LOOP_MAX 1000000 //1,000,000,000

void setup() {

  Serial.begin();

  //set up qt-py status LED
  //This board has a power control pin, we must set it to output and high in order to enable the NeoPixels.
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
  pixels.begin(); //INITIALIZE NeoPixel strip object
  pixels.setBrightness(100);
  pixels.fill(0x0000FF); //status led to default blue
  pixels.show();
  
  //connect to the i2c devices
  //&Wire1 is the STEMMA connector on QT Py RP2040.
  if (!matrix.begin(0x70, &Wire1)) {
    Serial.println("Matrix not found");
    pixels.fill(0xFF0000); //status LED to red to indicate failure
    pixels.show();
    while (1); //halt
  }
  if(!ledDriver.begin(0x58, &Wire1)) {
    Serial.println("AW9523 not found");
    pixels.fill(0xFF0000); //status LED to red to indicate failure
    pixels.show();
    while (1); //halt
  }

  for (uint8_t i=0; i<NUM_LEDS; i++) {
    ledDriver.pinMode(leds[i], AW9523_LED_MODE); //set constant current drive mode
    ledDriver.analogWrite(leds[i], 0); //leds off
  }
  for (uint8_t i=0; i<2; i++) {
    ledDriver.pinMode(sign[i], AW9523_LED_MODE); //set constant current drive mode
    ledDriver.analogWrite(sign[i], 0); //sign off
  }
  
  //turn on all the leds
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    ledDriver.analogWrite(leds[i], MAX_BRIGHTNESS);
  }
  ledDriver.analogWrite(SIGN_TOP, MAX_BRIGHTNESS);
  ledDriver.analogWrite(SIGN_BOTTOM, MAX_BRIGHTNESS);

  matrix.setBrightness(5); //brightness between 0 and 15
  matrix.setRotation(1);
  matrix.clear();
  matrix.drawBitmap(0,0, smile_bmp, 8,8, LED_ON);
  matrix.writeDisplay(); //display on matrix

  pixels.fill(0x00FF00); //status LED to green once setup is complete
  pixels.show();
}

void loop() {
  // for (uint8_t i=0; i<NUM_PINS; i++) { //For each pin...
  //   //Calc sine wave, phase offset for each pin, with gamma correction.
  //   //If using red, green, blue nOOds, you'll get a cycle of hues.
    // float phase = (millis() / 1000.0 - 2.0 * i / (float)NUM_PINS) * M_PI;
  //   int brightness = int(pow((sin(phase) + 1.0) * 0.5, GAMMA) * 255 + 0.5);
  //   ledDriver.analogWrite(pins[i], brightness);
  // }

  //GAMMA is what is causing the flat spot on the bottom
  //and the phase needs a much larger shift
  //Should use millis()/1000 * pi (period of 1s) and add like 500 to it for bottom
  float phase = (loopCounter / 5000.0) * M_PI;
  int brightness = int(pow((sin(phase) + 1.0) * 0.5, GAMMA) * MAX_BRIGHTNESS + 0.5);
  Serial.print("BrightnessTop:"); Serial.print(brightness); Serial.print(","); Serial.print("Max:"); Serial.print(MAX_BRIGHTNESS); Serial.print(",");
  Serial.print("PhaseTop:"); Serial.print(phase); Serial.print(",");
  ledDriver.analogWrite(SIGN_TOP, brightness);

  phase = ((loopCounter / 5000.0) + 20.0) * M_PI;
  brightness = int(pow((sin(phase) + 1.0) * 0.5, GAMMA) * MAX_BRIGHTNESS + 0.5);
  Serial.print("PhaseBottom:"); Serial.print(phase); Serial.print(",");
  Serial.print("BrightnessBottom:"), Serial.println(brightness);
  ledDriver.analogWrite(SIGN_BOTTOM, brightness);
  
  



  loopCounter = (loopCounter + 1) % LOOP_MAX;
}

