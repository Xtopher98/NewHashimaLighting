#include <sys/_stdint.h>
#ifndef CONTROL_H
#define CONTROL_H

// #include "common/tusb_common.h"
// #include <cstdlib>
// #include <cstring>
// #include <sys/_intsup.h>
// #include <sys/_types.h>
// #include "SerialUSB.h"
// #include <iterator>
// #include <sys/_stdint.h>

#include <Arduino.h>
#include <Adafruit_AW9523.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include "constants.h"

Adafruit_AW9523 ledDriver;
Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

enum pattern { NONE, FLICKER, BREATHE, BLINK, PRINT, SCROLL };

//Control class enables ability to run multiple patterns on leds simultaneously
//without using sleep statements.
class Control {

  int pin;
  uint8_t brightness = MAX_BRIGHTNESS; // all leds are initially on
  uint8_t prevBrightness;
  uint32_t prevMillis;
  uint32_t curMillis;

  pattern activePattern;

  uint32_t interval;
  float phase;
  float period;
  bool repeat;

  uint16_t onTime;
  uint16_t offTime;
  uint8_t numBlinks;

  int16_t totalSteps;
  int16_t index;

  uint16_t flickerCooldown = 0;

  const uint8_t* bitmap;
  const uint8_t* endBitmap;
  bool doScroll;

  const char* text;

  void (*onComplete)();

  public:

  Control(int pin = 0) : pin(pin) {
      activePattern = NONE;
  }

  //Call once per main loop to check if the pattern needs to be updated
  void update() {
    curMillis = millis();
    if(curMillis - prevMillis > interval) {
      prevMillis = curMillis;
      switch(activePattern) {
        case FLICKER:
          if(flickerCooldown > 0)
            flickerCooldown--;
          else
            flickerUpdate();
          break;
        case BREATHE:
          breatheUpdate();
          break;
        case BLINK:
          blinkUpdate();
          break;
        case PRINT:
          printUpdate();
          break;
        case SCROLL:
          scrollUpdate();
          break;
        default:
          break;
      }
    }
  }

  //starts a period of time (duration) where the brightness is randomly changed every (_interval).
  //(cooldown) defines an optional minimum time between pattern triggering to be used if the pattern
  //is being triggered randomly 
  void flicker(int duration, int _interval, int cooldown = 0) {
    //don't start if a pattern is active
    if(activePattern != NONE) {
      return;
    }
    activePattern = FLICKER;
    randomSeed(millis()); // new random seed each time
    interval = _interval;
    totalSteps = duration / interval;
    prevBrightness = brightness;
    index = 0;
    flickerCooldown = cooldown;
  }

  //starts a single period of a cosign wave (bright -> off -> bright) that takes (_period)ms 
  //to complete
  //(_repeat) is used to restart the pattern at the end. Defaults to false
  void breathe(int _period, bool _repeat = false) {
    activePattern = BREATHE;

    interval = 1; 
    period = _period / 2;
    totalSteps = 2 * period / interval;
    index = 0;

    repeat = _repeat;
  }

  //starts a pattern of blinking with an on time of (_onTime) and an off time of (_offTime) that
  //repeats (_numBlinks) times. 
  //(_repeat) is used to restart the pattern at the end. Defaults to false
  void blink(uint16_t _onTime, uint16_t _offTime, uint8_t _numBlinks, bool _repeat = false) {
    activePattern = BLINK;

    onTime = _onTime;
    offTime = _offTime;
    numBlinks = _numBlinks;
    prevMillis = 0;
    brightness = HIGH;

    interval = onTime;
    repeat = _repeat;
  }

  //used with an 8x8 led matrix to scroll the given (_text) across the matrix. Scroll speed is
  //defined by (updateInterval)
  //(endBitmap) defines the image to be shown once the scrolling is complete
  void print(const char* _text, uint16_t updateInterval, const uint8_t map[], bool _doScroll = false, bool _repeat = false) {
    activePattern = PRINT;
    text = _text;

    endBitmap = map;
    doScroll = _doScroll;
    repeat = _repeat;
    
    interval = updateInterval;
    totalSteps = (strlen(text) * -6) - 1;
    index = 7;// start off the screen

    matrix.setTextSize(1);
    matrix.setTextWrap(false);
    matrix.setTextColor(LED_ON);

  }

  void scroll(uint16_t updateInterval, const uint8_t map[], const uint8_t endMap[], bool _repeat = false) {
    activePattern = SCROLL;

    bitmap = map;
    endBitmap = endMap;
    interval = updateInterval;
    totalSteps = -8;
    index = 5;
    repeat = _repeat;
  }

  private:

  //internal use at the end of a single step of a pattern to increment it to the next step 
  //or end the pattern
  void increment() {
    if((activePattern != PRINT && activePattern != SCROLL) && (++index >= totalSteps)) {
      index = 0;
      if(!repeat) {
        if(activePattern == FLICKER) {
          brightness = prevBrightness;
          ledDriver.analogWrite(pin, brightness);

        }
        activePattern = NONE;
      }

      if(onComplete != NULL) {
        onComplete(); 
      }
    }
    //print needs to decrement to scroll text the correct direction.
    else if(((activePattern == PRINT) || (activePattern == SCROLL)) && (--index < totalSteps)) {
      if(repeat) {
        index = 7;
      }
      else{
        if(doScroll) {
          doScroll = false;
          this->scroll(71, endBitmap, endBitmap, true); //71ms interval works great for the specific text and timing I'm using
        }
        else {
          index = 0;
          activePattern = NONE;
          matrix.clear();
          matrix.drawBitmap(0,0, endBitmap, 8,8, LED_ON);
          matrix.writeDisplay();
        }
      }
    }
  }

  //gets new random value and updates led's brightness
  void flickerUpdate() {
    brightness = random(255);
    ledDriver.analogWrite(pin, brightness);
    increment();
  }
  
  //calculates the next step of the cosign wave to updates led's brightness
  void breatheUpdate() {
    phase = (index / period) * M_PI;
    brightness = int(pow((cos(phase) + 1.0) * 0.5, GAMMA) * MAX_BRIGHTNESS + 0.5 );
    // Serial.print("phase:"); Serial.print(phase);Serial.print("index:"); Serial.print(index); Serial.print("brightness:"); Serial.println(brightness);
    ledDriver.analogWrite(pin, brightness);
    increment();
  }

  //turns led on or off
  void blinkUpdate() {
    if((brightness != LOW)) {
      prevMillis = curMillis;
      brightness = LOW;
      interval = offTime;
      ledDriver.analogWrite(pin, brightness);
    }
    else if((brightness == LOW)) {
      prevMillis = curMillis;
      brightness = HIGH;
      interval = onTime;
      ledDriver.analogWrite(pin, brightness);
      numBlinks--;
    }
    if(numBlinks == 0) {
      if(!repeat) {
        interval = 1;
        index = 0;
        repeat = true;
        activePattern = BREATHE;
      }
      if(onComplete != NULL) {
        onComplete(); 
      }
    }
  }

  //shifts the text by the correct amount and prints to matrix
  void printUpdate() {
      matrix.clear();
      matrix.setCursor(index, 0);
      matrix.print(text);
      matrix.writeDisplay();
      increment();
  }

  void scrollUpdate() {
    matrix.clear();
    matrix.drawBitmap(index,0, bitmap, 8,8, LED_ON); //draw bitmap shifted by the index
    matrix.writeDisplay();                          //write bitmap to display
    increment();
  }

};

#endif