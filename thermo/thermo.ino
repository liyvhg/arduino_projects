
/*
  The idea is to use a lilytiny to read a temperature sensor,
  then output the temperature using red LEDs such that they
  would approximate the look of a thermometer with red mercury.
*/

#include <Adafruit_NeoPixel.h>

#define NEO_PIN 1
#define LED_COUNT 24
#define MAX_BRIGHTNESS 255

#define ANALOG_MAX 1023

#define SENSOR_PIN A1
//3.3v reference if using teensy 3.x
#define V_REF 2.56
//0.5V at 0 degrees C, 0.75V at 25 C, and 10mV per degree C.
#define VOLT_AT_0C 0.5

long previousMillis = 0;
long interval = 1000;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
uint32_t white = strip.Color(MAX_BRIGHTNESS, MAX_BRIGHTNESS, MAX_BRIGHTNESS);
uint32_t off = strip.Color(0, 0, 0);

void setup(void) {
  pinMode(SENSOR_PIN, INPUT);
  strip.begin();
  strip.setBrightness(MAX_BRIGHTNESS/10);
  strip.show(); // Initialize all pixels to 'off'
}


void loop(void) {
  int sensorValue = analogRead(SENSOR_PIN);
  float voltage = sensorValue * V_REF / ANALOG_MAX;
  //converting from 10 mv per degree with 500 mV offset
  float temperatureC = (voltage - VOLT_AT_0C) * 100 ;
  float temperatureF = c2f(temperatureC);

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, off);
    }

    for (int i = 0; i < temperatureF / 10; i++) {
      strip.setPixelColor(i, white);
    }

  }
  strip.show();


}

float c2f(float c) {
  return (c * 9.0 / 5.0) + 32.0;
}

