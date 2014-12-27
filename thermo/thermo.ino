
/*
  The idea is to use a lilytiny to read a temperature sensor,
  then output the temperature using red LEDs such that they
  would approximate the look of a thermometer with red mercury.
*/

#include <Charlieplex.h>

#define ANALOG_MAX 1023
#define SENSOR_PIN A1
//3.3v reference if using teensy 3.x
#define V_REF 2.56
//0.5V at 0 degrees C, 0.75V at 25 C, and 10mV per degree C.
#define VOLT_AT_0C 0.5

#define NUMBER_OF_PINS 4
//define pins in the order you want to adress them
byte pins[] = {1, 2, 3, 4};

//initialize object
Charlieplex charlieplex = Charlieplex(pins, NUMBER_OF_PINS);

//individual 'pins' , adress charlieplex pins as you would adress an array
charliePin led2 = { 0 , 1 };
charliePin led3 = { 0 , 2 };
charliePin led1 = { 1 , 2 }; //led1 is indicated by current flow from 12 to 13

void setup(void) {
  pinMode(SENSOR_PIN, INPUT);
}

void loop(void) {
  int sensorValue = analogRead(SENSOR_PIN);
  float voltage = sensorValue * V_REF / ANALOG_MAX;
  //converting from 10 mv per degree with 500 mV offset
  float temperatureC = (voltage - VOLT_AT_0C) * 100 ;
  float temperatureF = c2f(temperatureC);

  charlieplex.charlieWrite(led1,HIGH);
  charlieplex.charlieWrite(led2,HIGH);
  charlieplex.charlieWrite(led3,HIGH);

}

float c2f(float c) {
  return (c * 9.0 / 5.0) + 32.0;
}


