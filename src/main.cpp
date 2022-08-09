/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */
#include <Arduino.h>
#include <BleKeyboard.h>
#include "esp_adc_cal.h"

#include <button.h>

#define BATLEVEL 35
#define BATTERY_LOW_LEVEL 90
#define CORRECTION 1.045f

#define ON_INTERVAL 250
#define OFF_INTERVAL 750
#define BATTERY_CHECK_INTERVAL 10000

#define LED_RED 27
#define LED_GREEN 26

#define BUTTON_LEFT 13
#define BUTTON_RIGHT 12
#define BUTTON_PAIRING 14
#define BUTTON_DEBOUNCE_INTERVAL 1000

#define KEY_LEFT_PEDDAL 97  // A
#define KEY_RIGHT_PEDDAL 98 // B

BleKeyboard* bleKeyboard;
Button leftButton(BUTTON_LEFT, BUTTON_DEBOUNCE_INTERVAL);
Button rightButton(BUTTON_RIGHT, BUTTON_DEBOUNCE_INTERVAL);

bool connected = false;
bool batteryLow = false;
int batteryLevel = 100;
unsigned long lastBatteryCheck;

void updateLEDs()
{
  if (connected)
  {
    digitalWrite(LED_GREEN, HIGH);
  }
  else
  {
    if (((millis() % (ON_INTERVAL + OFF_INTERVAL))) <= ON_INTERVAL)
    {
      digitalWrite(LED_GREEN, HIGH);
    }
    else
    {
      digitalWrite(LED_GREEN, LOW);
    }
  }

  if (batteryLow)
  {
    digitalWrite(LED_RED, HIGH);
  }
  else
  {
    digitalWrite(LED_RED, LOW);
  }
}

int getBatteryLevel()
{
  int rawValue = analogRead(BATLEVEL) * 2; // * 3.3 * 2.0 * CORRECTION)/ 4095.0f );
  Serial.println(rawValue);

  esp_adc_cal_characteristics_t adc_chars;

  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);

  int correctedValue = (((float)esp_adc_cal_raw_to_voltage(rawValue, &adc_chars)) / 33.0f);

  Serial.println(correctedValue);

  return correctedValue;
}

void batteryCheck()
{
  lastBatteryCheck = millis();
  batteryLevel = getBatteryLevel();
  batteryLow = (batteryLevel < BATTERY_LOW_LEVEL);

  Serial.println(batteryLevel);
  if(connected) 
  {
    bleKeyboard->setBatteryLevel(batteryLevel);
  }
}

void buttonLeftPressed(Button * button)
{
  Serial.println("buttonLeftPressed");
  if (connected)
  {
    Serial.println("press");
    bleKeyboard->press(KEY_LEFT_PEDDAL);
  }
}

void buttonLeftReleased(Button * button)
{
  Serial.println("buttonLeftReleased");
  if (connected)
  {
    Serial.println("release");
    bleKeyboard->release(KEY_LEFT_PEDDAL);
  }
}

void buttonRightPressed(Button * button)
{
  Serial.println("buttonRightPressed");
  if (connected)
  {
    Serial.println("press");
    bleKeyboard->press(KEY_RIGHT_PEDDAL);
  }
}

void buttonRightReleased(Button * button)
{
  Serial.println("buttonRightReleased");
  if (connected)
  {
    Serial.println("release");
    bleKeyboard->release(KEY_RIGHT_PEDDAL);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BATLEVEL, INPUT);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);

  leftButton.setPressedCallback(&buttonLeftPressed);
  leftButton.setReleasedCallback(&buttonLeftReleased);
  rightButton.setPressedCallback(&buttonRightPressed);
  rightButton.setReleasedCallback(&buttonRightReleased);

  connected = false;
  batteryCheck();
  bleKeyboard = new BleKeyboard("Page Turner", "ByForero", batteryLevel);
  bleKeyboard->begin();
}

void loop()
{
  bool doBatteryCheck = ((millis() - BATTERY_CHECK_INTERVAL) >= lastBatteryCheck);

  if (doBatteryCheck)
  {
    batteryCheck();
  }

  if (bleKeyboard->isConnected())
  {
    if (!connected)
    {
      Serial.println("Connected");
      connected = true;
      bleKeyboard->releaseAll();
    }
  }
  else
  {
    if (connected)
    {
      Serial.println("Disconnected");
      connected = false;
    }
  }

  leftButton.loop();
  rightButton.loop();

  updateLEDs();
}