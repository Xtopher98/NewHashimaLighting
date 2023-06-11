#include "common/tusb_common.h"
#include <cstdlib>
#ifndef CONTROL_H
#define CONTROL_H

#include <cstring>
#include <sys/_intsup.h>
#include <sys/_types.h>
#include "SerialUSB.h"
#include <iterator>
#include <sys/_stdint.h>

#include <Arduino.h>
#include <Adafruit_AW9523.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include "constants.h"

Adafruit_AW9523 ledDriver;
Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

enum pattern { NONE, FLICKER, BREATHE, BLINK, PRINT };

class Control {

  int pin;
  uint8_t brightness = 255;
  uint8_t prevBrightness;
  uint32_t prevMillis;
  uint32_t curMillis;

  pattern activePattern;

  int interval;
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

  const char* text;

  void (*onComplete)();

  public:

  Control(int pin = 0) : pin(pin) {
      activePattern = NONE;
  }

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
        default:
          break;
      }
    }
  }

  void flicker(int duration, int _interval, int cooldown) {
    //don't start if a pattern is active
    if(activePattern != NONE) {
      return;
    }
    activePattern = FLICKER;
    randomSeed(millis());
    interval = _interval;
    totalSteps = duration / interval;
    prevBrightness = brightness;
    index = 0;
    flickerCooldown = cooldown;
  }

  void breathe(int _period, bool _repeat = false) {
    // if(activePattern != NONE) {
    //   return;
    // }
    activePattern = BREATHE;

    interval = 1; 
    period = _period / 2;
    totalSteps = 2 * period / interval;
    index = 0;

    repeat = _repeat;
  }

  void blink(uint16_t _onTime, uint16_t _offTime, uint8_t _numBlinks, bool _repeat = false, void (*callback)() = NULL) {
    activePattern = BLINK;

    onTime = _onTime;
    offTime = _offTime;
    numBlinks = _numBlinks;
    prevMillis = 0;
    brightness = HIGH;
    onComplete = callback;

    interval = onTime;
    repeat = _repeat;
  }

  void print(const char* _text, uint16_t updateInterval, const uint8_t endBitmap[]) {
    activePattern = PRINT;
    text = _text;

    bitmap = endBitmap;
    
    interval = updateInterval;
    totalSteps = (strlen(text) * -6) - 1;
    index = 7;// start off the screen

    matrix.setTextSize(1);
    matrix.setTextWrap(false);
    matrix.setTextColor(LED_ON);

  }

  private:

  void increment() {
    if((activePattern != PRINT) && (++index >= totalSteps)) {
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
    else if((activePattern == PRINT) && (--index < totalSteps)) {
      index = 0;
      activePattern = NONE;
      matrix.clear();
      matrix.drawBitmap(0,0, bitmap, 8,8, LED_ON);
      matrix.writeDisplay();
    }
  }

  void flickerUpdate() {
    brightness = random(255);
    ledDriver.analogWrite(pin, brightness);
    increment();
  }
  
  void breatheUpdate() {
    phase = (index / period) * M_PI;
    brightness = int(pow((cos(phase) + 1.0) * 0.5, GAMMA) * MAX_BRIGHTNESS + 0.5 );
    // Serial.print("phase:"); Serial.print(phase);Serial.print("index:"); Serial.print(index); Serial.print("brightness:"); Serial.println(brightness);
    ledDriver.analogWrite(pin, brightness);
    increment();
  }

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

  void printUpdate() {
      matrix.clear();
      matrix.setCursor(index, 0);
      matrix.print(text);
      matrix.writeDisplay();
      increment();
  }

};

#endif