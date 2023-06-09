#ifndef CONTROL_H
#define CONTROL_H



class Control {

    int pin;
    int curBrightness;
    unsigned long prevMillis;

    bool controlActive;


public:
    Control(int pin) : pin(pin) {
        curBrightness = 0;
        controlActive = false;
    }

    void flicker() {

    }

    void breath() {

    }

    void blink() {
        
    }
}

#endif