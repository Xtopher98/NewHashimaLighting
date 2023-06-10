#ifndef CONTROL_H
#define CONTROL_H

enum pattern { NONE, FLICKER, BREATHE, BLINK };

class Control {

    int pin;
    int curBrightness;
    unsigned long lastUpdate;
    int interval;

    pattern activePattern;


public:
    Control(int pin) : pin(pin) {
        curBrightness = 0;
        lastUpdate = 0;
        activePattern = NONE;
    }

    void update();

    void flicker();
    void flickerUpdate();

    void breathe();
    void breatheUpdate();

    void blink();
    void blinkUpdate();
};

#endif