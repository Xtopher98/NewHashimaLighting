#include <Arduino.h>
#include <Wire.h>
// #include <Adafruit_AW9523.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_LEDBackpack.h>
#include <Adafruit_NeoPixel.h>
// #include "constants.h"
#include "control.h"

// #define SIGN_TOP 1
// #define SIGN_BOTTOM 2
// #define DOT_RIGHT 6
// #define DOT_LEFT 7
// #define ARCH_RIGHT 12
// #define ARCH_LEFT 13
// #define CORNER_RIGHT 14
// #define CORNER_LEFT 15

// #define MIN_BRIGHTNESS 0
// #define MAX_BRIGHTNESS 255

// uint8_t leds[] = { DOT_RIGHT, DOT_LEFT, ARCH_RIGHT, ARCH_LEFT, CORNER_RIGHT, CORNER_LEFT };
// uint8_t sign[] = { SIGN_TOP, SIGN_BOTTOM };
// #define NUM_LEDS (sizeof(leds) / sizeof(leds[0]))

// #define GAMMA 2.6 //for perceptually-linear brightness

Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
// Adafruit_AW9523 ledDriver;
// Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

Control signTop(SIGN_TOP);
Control signBottom(SIGN_BOTTOM);

Control dotRight(DOT_RIGHT);
Control dotLeft(DOT_LEFT);

Control mat(-1);


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
    ledDriver.analogWrite(leds[i], MAX_BRIGHTNESS); //leds off
  }
  for (uint8_t i=0; i<2; i++) {
    ledDriver.pinMode(sign[i], AW9523_LED_MODE); //set constant current drive mode
    ledDriver.analogWrite(sign[i], MAX_BRIGHTNESS); //sign off
  }
  
  dotLeft.breathe(1000, true);
  dotRight.breathe(1000, true);
 

  matrix.setBrightness(5); //brightness between 0 and 15
  matrix.setRotation(1);
  matrix.clear();
  matrix.drawBitmap(0,0, smile_bmp, 8,8, LED_ON);
  matrix.writeDisplay();

  pixels.fill(0x00FF00); //status LED to green once setup is complete
  pixels.show();
}

void loop() {

  if(millis() % 5000 == 0) 
    signTop.breathe(1000);
  if(millis() % 5000 == 300)
    signBottom.breathe(1000);

  if(millis() % 10000 == 2000) {
    dotLeft.blink(200, 100, 21);
    dotRight.blink(200, 100, 21);
    mat.print("Scanning in progress", 50);
  }

  signTop.update();
  signBottom.update();
  dotLeft.update();
  dotRight.update();
  mat.update();


  // float phase = (millis() / 2000.0) * M_PI;
  // int brightness = int(pow((cos(phase) + 1.0) * 0.5, 1) * MAX_BRIGHTNESS + 0.5);
  // ledDriver.analogWrite(SIGN_TOP, brightness);

  // phase = ((millis() - 300) / 2000.0) * M_PI;
  // brightness = int(pow((cos(phase) + 1.0) * 0.5, 1) * MAX_BRIGHTNESS + 0.5);
  // ledDriver.analogWrite(SIGN_BOTTOM, brightness);
  
}

