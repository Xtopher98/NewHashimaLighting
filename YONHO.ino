#include <Adafruit_NeoPixel.h>
#include "control.h"


//onboard neopixel used as status indicator
Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

//setup control for each led where it's needed
Control signTop(SIGN_TOP);
Control signBottom(SIGN_BOTTOM);

Control dotLeft(DOT_LEFT);
Control dotRight(DOT_RIGHT);

Control archLeft(ARCH_LEFT);
Control archRight(ARCH_RIGHT);

Control mat(-1); //no pin it's over &Wire1


void setup() {

  Serial.begin();

  //set up qt-py status LED
  //This board has a power control pin, we must set it to output and high in order to enable the NeoPixels.
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
  pixels.begin();
  pixels.setBrightness(100);
  pixels.fill(0x0000FF); //status led to default blue
  pixels.show();
  
  //connect to the i2c devices
  //&Wire1 is the STEMMA connector on QT Py RP2040.
  if (!matrix.begin(0x70, &Wire1)) {
    Serial.println("Matrix not found");
    pixels.fill(0xFF0000); //status LED to red to indicate failure
    pixels.show();
    while (1); //give up
  }
  if(!ledDriver.begin(0x58, &Wire1)) {
    Serial.println("AW9523 not found");
    pixels.fill(0xFF0000); //status LED to red to indicate failure
    pixels.show();
    while (1); //give up
  }

  //initialize AW9523 to LED mode for each pin and turn LEDs on
  for (uint8_t i=0; i<NUM_LEDS; i++) {
    ledDriver.pinMode(leds[i], AW9523_LED_MODE); //set constant current drive mode
    ledDriver.analogWrite(leds[i], MAX_BRIGHTNESS); //leds on
  }
  
  //start dot's default pattern of breathing
  dotLeft.breathe(1000, true);
  dotRight.breathe(1000, true);
 
  matrix.setBrightness(5); //brightness between 0 and 15
  matrix.setRotation(1);   //ensure bitmaps and text are displayed in the right orientation
  matrix.clear();
  matrix.drawBitmap(0,0, smile_bmp, 8,8, LED_ON); //draw default bitmap
  matrix.writeDisplay();                          //write bitmap to display

  pixels.fill(0x00FF00); //status LED to green once setup is complete
  pixels.show();
  randomSeed(millis()); //seed the random number generator
}


void loop() {

  //trigger sign to fade out and in every 5 seconds
  if(millis() % 5000 == 0)
    signTop.breathe(1000);
  if(millis() % 5000 == 300) // offset breathe by 0.3s from top of sign
    signBottom.breathe(1000);
  

  //start of 1min loop
  if(millis() % 60000 == 1000) {
    mat.print("New Hashima Service", 50, smile_bmp);
  }

  //trigger scanning patterns at 20s and 40s
  if(millis() % 60000 == 20000 || millis() % 60000 == 40000) {
    dotLeft.blink(300, 100, 14);
    dotRight.blink(300, 100, 14);
    mat.print("Scanning: Scanning:", 50, left_bmp, true); //triggeres the bmp to scroll after the text is done
  }

  //randomly flicker arches and sign
  if(millis() % random(750000) == 0) {
    archLeft.flicker(500, 10, 1000);
  }
  if(millis() % random(750000) == 0) {
    archRight.flicker(500, 10, 1000);
  }
  if(millis() % random(500000) == 0) {
    signTop.flicker(500, 10, 100);
    signBottom.flicker(500, 10, 100);
  }

  //update all the things!
  signTop.update();
  signBottom.update();
  dotLeft.update();
  dotRight.update();
  archLeft.update();
  archRight.update();
  mat.update();
}

