#include <Wire.h>
#include <Adafruit_AW9523.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_NeoPixel.h>

#define SIGN_TOP 1
#define SIGN_BOTTOM 2
#define DOT_RIGHT 6
#define DOT_LEFT 7
#define ARCH_RIGHT 12
#define ARCH_LEFT 13
#define CORNER_RIGHT 14
#define CORNER_LEFT 15

uint8_t pins[] = { SIGN_TOP, SIGN_BOTTOM, DOT_RIGHT, DOT_LEFT, ARCH_RIGHT, ARCH_LEFT, CORNER_RIGHT, CORNER_LEFT }; // List of pins, one per nOOd
#define NUM_PINS (sizeof(pins) / sizeof(pins[0]))
#define GAMMA 2.6                // For perceptually-linear brightness

Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

Adafruit_AW9523 ledDriver;
Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

void setup() {

#if defined(NEOPIXEL_POWER)
  // If this board has a power control pin, we must set it to output and high
  // in order to enable the NeoPixels. We put this in an #if defined so it can
  // be reused for other boards without compilation errors
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
#endif

  pixels.begin(); // INITIALIZE NeoPixel strip object
  pixels.setBrightness(100);
  pixels.fill(0x0000FF); //status led to default blue
  pixels.show();
  
  //connect to the i2c devices
  // &Wire1 is the STEMMA connector on QT Py RP2040.
  if (!matrix.begin(0x70, &Wire1)) {
    Serial.println("Matrix not found");
    pixels.fill(0xFF0000); //status LED to red to indicate failure
    pixels.show();
    while (1); // halt
  }
  if(!ledDriver.begin(0x58, &Wire1)) {
    Serial.println("AW9523 not found");
    pixels.fill(0xFF0000); //status LED to red to indicate failure
    pixels.show();
    while (1); // halt
  }

  for (uint8_t i=0; i<NUM_PINS; i++) {
    ledDriver.pinMode(pins[i], AW9523_LED_MODE); //set constant current drive mode
    ledDriver.analogWrite(pins[i], 0); // nOOd off
  }

  // from 8x8 matrix example code
  matrix.setRotation(0);
  
  matrix.clear();
  matrix.drawRect(0,0, 8,8, LED_ON);
  matrix.fillRect(2,2, 4,4, LED_ON);
  matrix.writeDisplay();  // write the changes we just made to the display

  pixels.fill(0x00FF00); //status LED to green once setup is complete
  pixels.show();
}




void loop() {
  for (uint8_t i=0; i<NUM_PINS; i++) { // For each pin...
    // Calc sine wave, phase offset for each pin, with gamma correction.
    // If using red, green, blue nOOds, you'll get a cycle of hues.
    float phase = (millis() / 1000.0 - 2.0 * i / (float)NUM_PINS) * M_PI;
    int brightness = int(pow((sin(phase) + 1.0) * 0.5, GAMMA) * 255 + 0.5);
    ledDriver.analogWrite(pins[i], brightness);
  }

  


}