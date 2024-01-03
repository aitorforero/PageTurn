#include <Arduino.h>
#include <BleKeyboard.h>
#include "esp_adc_cal.h"

#include <button.h>

#define BATTERY_CHANNEL ADC1_CHANNEL_7
#define BATTERY_ATTEN ADC_ATTEN_DB_11
#define BATTERY_WIDTH_BIT ADC_WIDTH_BIT_12
#define DEFAULT_VREF    1100
#define BATTERY_MAX_VOL 4200.0
#define BATTERY_MIN_VOL 3000.0
#define BATTERY_LOW_LEVEL 20
#define BATTERY_CHECK_INTERVAL 10000

#define NOT_CONNECTED_OFF_INTERVAL 750
#define NOT_CONNECTED_ON_INTERVAL 250
#define LOW_BATTERY_OFF_INTERVAL 250
#define LOW_BATTERY_ON_INTERVAL 750

#define LED 12

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
static esp_adc_cal_characteristics_t *adc_chars;
unsigned long millisPairingPressed = 0;

int notConnectedOffIntervalStart = 0;
int notConnectedOffIntervalEnd = notConnectedOffIntervalStart + NOT_CONNECTED_OFF_INTERVAL;
int lowBatteryOffIntervalStart = notConnectedOffIntervalEnd + NOT_CONNECTED_ON_INTERVAL;
int lowBatteryOffIntervalEnd = lowBatteryOffIntervalStart + LOW_BATTERY_OFF_INTERVAL;

void updateLEDs()
{
  int moment = millis() % (lowBatteryOffIntervalEnd + LOW_BATTERY_ON_INTERVAL);

 

  if(!connected && moment >= notConnectedOffIntervalStart && moment < notConnectedOffIntervalEnd ) 
  {
    digitalWrite(LED, LOW);
  }
  else if(batteryLow && moment >= lowBatteryOffIntervalStart && moment < lowBatteryOffIntervalEnd) 
  {
    digitalWrite(LED, LOW);
  }
  else
  {
    digitalWrite(LED, HIGH);
  }
}
void setupBatteryCheck()
{
  adc1_config_width(BATTERY_WIDTH_BIT);
  adc1_config_channel_atten(BATTERY_CHANNEL, BATTERY_ATTEN);

  adc_chars = (esp_adc_cal_characteristics_t * )calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, BATTERY_ATTEN, BATTERY_WIDTH_BIT, DEFAULT_VREF, adc_chars);

  if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
      printf("Characterized using Two Point Value\n");
  } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
      printf("Characterized using eFuse Vref\n");
  } else {
      printf("Characterized using Default Vref\n");
  }

  lastBatteryCheck = millis();

}

int getBatteryLevel()
{
  int raw = adc1_get_raw(BATTERY_CHANNEL); // The battery is connected with a voltage divider (100K -|- 100K)
  int correctedValue = esp_adc_cal_raw_to_voltage(raw, adc_chars) * 2;

  int level = (correctedValue - BATTERY_MIN_VOL) / (BATTERY_MAX_VOL - BATTERY_MIN_VOL) * 100.0 ;

  if(level > 100)
  {
    level = 100;
  } 
  else if(level < 0) 
  {
    level = 0;    
  }
 
  printf("Raw: %d\tVoltage: %dmV\tPorcentage: %d%\n", raw, correctedValue, level);

  return level;
}

void batteryCheck()
{
  if((millis() - lastBatteryCheck) >=  BATTERY_CHECK_INTERVAL)
  {
    lastBatteryCheck = millis();
    batteryLevel = getBatteryLevel();
    batteryLow = (batteryLevel < BATTERY_LOW_LEVEL);

    if(connected) 
    {
      bleKeyboard->setBatteryLevel(batteryLevel);
    }    
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

  pinMode(LED, OUTPUT);
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
  setupBatteryCheck();
  bleKeyboard = new BleKeyboard("Page Turner", "ByForero", batteryLevel);
  bleKeyboard->begin();
}

void loop()
{


  batteryCheck();


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