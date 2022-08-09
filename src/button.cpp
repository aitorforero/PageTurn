#include <button.h>
#include <Arduino.h>

Button::Button(uint8_t pin, long debounceTime)
{
    this->pin = pin;
    this->readInterval = debounceTime / 8;
    this->lastRead = millis();
    this->mask = 0;
    this->pressed = false;
}

void Button::loop()
{
    int readValue = digitalRead(this->pin);

    long currentMillis = millis();
    if((currentMillis - this->lastRead) >= this->readInterval)
    {
        this->mask = this->mask << 1 | readValue;
        if(this->mask == 0x0 && !this->pressed)
        {
            this->pressed = true;
            (*(this->pressedCallback))(this);
        }
        else if (this->mask == 0xFF && this->pressed)
        {
            this->pressed = false;
            (*(this->releasedCallback))(this);
        }
    }
}

void Button::setPressedCallback(ButtonCallback_t callback)
{
    this->pressedCallback = callback;
}

void Button::setReleasedCallback(ButtonCallback_t callback)
{
    this->releasedCallback = callback;
};
