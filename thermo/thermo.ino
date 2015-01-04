
#include <ADC.h>

#define ANALOG_MAX 1023

#define SENSOR_PIN 14
#define LED_PIN 13
#define V_REF 3.3

//0.5V at 0 degrees C, 0.75V at 25 C, and 10mV per degree C.
#define VOLT_AT_0C 0.5

long previousMillis = 0;
long interval = 1000;


ADC *adc = new ADC(); // adc object

void setup(void) {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);

  pinMode(SENSOR_PIN, INPUT);
  adc->setAveraging(1); // set number of averages
  adc->setResolution(12); // set bits of resolution

}


void loop(void) {
  int sensorValue = adc->analogRead(SENSOR_PIN);
  float voltage = sensorValue * V_REF / adc->getMaxValue(ADC_0);
  //converting from 10 mv per degree with 500 mV offset
  float temperatureC = (voltage - VOLT_AT_0C) * 100 ;
  float f = c2f(temperatureC);

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    if (f < 65) {
      //off
    } else if (f >= 65 && f < 70) {
      blink();
    } else if (f >= 70 && f < 75) {
      blink();
      blink();
    } else {
      blink();
      blink();
      blink();
    }
  }
}

void blink() {
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(100);
}

float c2f(float c) {
  return (c * 9.0 / 5.0) + 32.0;
}

