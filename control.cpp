#include <Arduino.h>

#include "control.h"

void Control::update() {
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

void Control::flicker() {

}
void Control::flickerUpdate() {

}

void Control::breathe() {

}

void Control::breatheUpdate() {
  
}

void Control::blink() {

}

void Control::blinkUpdate() {
  
}