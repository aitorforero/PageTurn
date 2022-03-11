#ifndef PAGETURN_BUTTON
#define PAGETURN_BUTTON

#include <Arduino.h>

class Button;

typedef void (* ButtonCallback_t)(Button*);

class Button
{
private:
    uint8_t pin;
    long readInterval;
    long lastRead;
    uint8_t mask;
    bool pressed;
    ButtonCallback_t pressedCallback;
    ButtonCallback_t releasedCallback;

public:
    Button(uint8_t pin, long debounceTime);
    void loop();
    void setPressedCallback(ButtonCallback_t callback);
    void setReleasedCallback(ButtonCallback_t callback);
};

#endif