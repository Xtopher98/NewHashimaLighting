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
    float phase;
    unsigned long lastUpdate;
    int interval;
    float period;

    bool repeat;

    pattern activePattern;

    uint16_t totalSteps;
    uint16_t index;

    void (*onComplete)();

public:

    Control(int pin, bool repeat = false) : pin(pin), repeat(repeat) {
        activePattern = NONE;
    }

    void update(){
      if(millis() - lastUpdate > interval) {
        lastUpdate = millis();
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

    void breathe(uint8_t i, int _period) {
      activePattern = BREATHE;
      interval = i; 
      period = _period;
      totalSteps = 2 * period / interval;
      index = 1;
    }

    void blink() {

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
      brightness = int( ((cos(phase) + 1.0) * 0.5) * MAX_BRIGHTNESS + 0.5 );
      // Serial.print("phase:"); Serial.print(phase);Serial.print("index:"); Serial.print(index); Serial.print("brightness:"); Serial.println(brightness);
      ledDriver.analogWrite(pin, brightness);
      increment();
    }

    void blinkUpdate() {

    }

};

#endif