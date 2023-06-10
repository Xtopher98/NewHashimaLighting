#include <sys/_types.h>
#include "SerialUSB.h"
#include <iterator>
#include <sys/_stdint.h>
#ifndef CONTROL_H
#define CONTROL_H

#include <Arduino.h>
#include <Adafruit_AW9523.h>
#include "constants.h"

Adafruit_AW9523 ledDriver;

enum pattern { NONE, FLICKER, BREATHE, BLINK };

class Control {

  int pin;
  int brightness;
  uint32_t prevMillis;

  pattern activePattern;

  int interval;
  float phase;
  float period;
  bool repeat;

  uint16_t onTime;
  uint16_t offTime;
  uint8_t numBlinks;

  uint16_t totalSteps;
  uint16_t index;

  void (*onComplete)();

public:

    Control(int pin) : pin(pin){
        activePattern = NONE;
    }

    void update(){
      if(millis() - prevMillis > interval) {
        prevMillis = millis();
        switch(activePattern) {
          case FLICKER:
            flickerUpdate();
            break;
          case BREATHE:
            breatheUpdate();
            break;
          case BLINK:
            blinkUpdate();
            break;
          default:
            break;
        }
      }
    }

    void flicker() {

    }

    void breathe(int _period, bool _repeat = false){
      activePattern = BREATHE;

      interval = 1; 
      period = _period;
      totalSteps = 2 * period / interval;
      index = 0;

      repeat = _repeat;
    }

    void blink(uint16_t _onTime, uint16_t _offTime, uint8_t _numBlinks, bool _repeat = false, void (*callback)() = NULL){
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

private:

    void increment() {
      if(++index >= totalSteps) {
        index = 0;
        if(!repeat)
          activePattern = NONE;

        if(onComplete != NULL) {
          onComplete(); 
        }
      }
    }

    void flickerUpdate() {

    }
    
    void breatheUpdate() {
      phase = (index / period) * M_PI;
      brightness = int(pow((cos(phase) + 1.0) * 0.5, GAMMA) * MAX_BRIGHTNESS + 0.5 );
      // Serial.print("phase:"); Serial.print(phase);Serial.print("index:"); Serial.print(index); Serial.print("brightness:"); Serial.println(brightness);
      ledDriver.analogWrite(pin, brightness);
      increment();
    }

    void blinkUpdate() {
      unsigned long curMillis = millis();

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

};

#endif