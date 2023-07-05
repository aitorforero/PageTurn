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

#define LED_RED 14
#define LED_GREEN 12

#define BUTTON_1 4
#define BUTTON_2 0
#define BUTTON_3 2
#define BUTTON_4 15

#define BUTTON_PAIRING 13
#define BUTTON_DEBOUNCE_INTERVAL 1000
#define BUTTON_PAIRING_PRESSED_INTERVAL 3000

#define KEY_BUTTON_1 97  // A
#define KEY_BUTTON_2 98 // B
#define KEY_BUTTON_3 99 // C
#define KEY_BUTTON_4 100 // D

BleKeyboard* bleKeyboard;
Button button1(BUTTON_1, BUTTON_DEBOUNCE_INTERVAL);
Button button2(BUTTON_2, BUTTON_DEBOUNCE_INTERVAL);
Button button3(BUTTON_3, BUTTON_DEBOUNCE_INTERVAL);
Button button4(BUTTON_4, BUTTON_DEBOUNCE_INTERVAL);
Button buttonPairing(BUTTON_PAIRING, BUTTON_DEBOUNCE_INTERVAL);

bool connected = false;
bool batteryLow = false;
int batteryLevel = 100;
unsigned long lastBatteryCheck;
unsigned long millisPairingPressed = 0;

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

void remove_all_bonded_devices(void)
{
    int dev_num = esp_ble_get_bond_device_num();
    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    for (int i = 0; i < dev_num; i++) {
        esp_ble_remove_bond_device(dev_list[i].bd_addr);
    }

    free(dev_list);
}


void button1Pressed(Button * button)
{
  Serial.println("button1Pressed");
  if (connected)
  {
    Serial.println("press");
    bleKeyboard->press(KEY_BUTTON_1);
  }
}

void button1Released(Button * button)
{
  Serial.println("button1Released");
  if (connected)
  {
    Serial.println("release");
    bleKeyboard->release(KEY_BUTTON_1);
  }
}

void button2Pressed(Button * button)
{
  Serial.println("button2Pressed");
  if (connected)
  {
    Serial.println("press");
    bleKeyboard->press(KEY_BUTTON_2);
  }
}

void button2Released(Button * button)
{
  Serial.println("button2Released");
  if (connected)
  {
    Serial.println("release");
    bleKeyboard->release(KEY_BUTTON_2);
  }
}

void button3Pressed(Button * button)
{
  Serial.println("button3Pressed");
  if (connected)
  {
    Serial.println("press");
    bleKeyboard->press(KEY_BUTTON_3);
  }
}

void button3Released(Button * button)
{
  Serial.println("button3Released");
  if (connected)
  {
    Serial.println("release");
    bleKeyboard->release(KEY_BUTTON_3);
  }
}

void button4Pressed(Button * button)
{
  Serial.println("button4Pressed");
  if (connected)
  {
    Serial.println("press");
    bleKeyboard->press(KEY_BUTTON_4);
  }
}

void button4Released(Button * button)
{
  Serial.println("button4Released");
  if (connected)
  {
    Serial.println("release");
    bleKeyboard->release(KEY_BUTTON_4);
  }
}

void buttonPairingPressed(Button * button)
{
  Serial.println("buttonPairingPressed");
  millisPairingPressed = millis();
}

void buttonPairingReleased(Button * button)
{
  Serial.println("buttonPairingReleased");
  unsigned long millisSincePairingPressed = millis() - millisPairingPressed;
  Serial.printf("Millis since Pairing Pressed = %i", millisSincePairingPressed);

  Serial.println();

  if(millisSincePairingPressed > BUTTON_PAIRING_PRESSED_INTERVAL)
  {
    Serial.println("Reset Pairing");
    remove_all_bonded_devices();
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BATLEVEL, INPUT);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(BUTTON_PAIRING, INPUT_PULLUP);

  button1.setPressedCallback(&button1Pressed);
  button1.setReleasedCallback(&button1Released);
  button2.setPressedCallback(&button2Pressed);
  button2.setReleasedCallback(&button2Released);
  button3.setPressedCallback(&button3Pressed);
  button3.setReleasedCallback(&button3Released);
  button4.setPressedCallback(&button4Pressed);
  button4.setReleasedCallback(&button4Released);
  buttonPairing.setPressedCallback(&buttonPairingPressed);
  buttonPairing.setReleasedCallback(&buttonPairingReleased);

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

  button1.loop();
  button2.loop();
  button3.loop();
  button4.loop();
  buttonPairing.loop();

  updateLEDs();
}