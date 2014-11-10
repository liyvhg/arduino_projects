
#include <Adafruit_NeoPixel.h>
#include <Time.h>

#define NEO_PIN 11
#define LED_PIN 13
#define LED_COUNT 24
#define MAX_BRIGHTNESS 255
#define UTC_OFFSET 0 //0 when using processing sketch

//Max of unit
#define HOURS 12.0
#define MINUTES 60.0
#define SECONDS 60.0

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
uint32_t red = strip.Color(255, 0, 0);
uint32_t green = strip.Color(0, 255, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t magenta = red || blue;
uint32_t off = strip.Color(0, 0, 0);

inline int positive_modulo(int i, int n) {
  return (i % n + n) % n;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  strip.begin();
  strip.setBrightness(MAX_BRIGHTNESS/10);
  strip.show(); // Initialize all pixels to 'off'

  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  setSyncProvider( requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");

}

void loop() {

  if (Serial.available()) {
    processSyncMessage();
  }

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, off);
  }

  if (timeStatus() == timeSet) {
    digitalWrite(LED_PIN, LOW);  // LED off if synced
    int hourLed = positive_modulo(hourFormat12() + UTC_OFFSET, HOURS) * (LED_COUNT / HOURS);
    int minuteLed = minute() / (MINUTES / LED_COUNT);
    int secondLed = second() / (SECONDS / LED_COUNT);
    int secondRemain = second() % (int)(SECONDS / LED_COUNT) + 1;
    uint32_t partialBlue = strip.Color(0, 0, MAX_BRIGHTNESS / secondRemain);

    strip.setPixelColor(hourLed, red | strip.getPixelColor(hourLed));
    strip.setPixelColor(minuteLed, green | strip.getPixelColor(minuteLed));
    strip.setPixelColor(secondLed, partialBlue | strip.getPixelColor(secondLed));

    /*
    if (second() > 55) {
      Serial.println("hour: " + String(hourFormat12()) + " -> " + String(hourLed));
      Serial.println("minute: " + String(minute()) + " -> " + String(minuteLed));
      Serial.println("second: " + String(second()) + " -> " + String(secondLed));
    }
    */

    strip.show();
  } else {
    digitalWrite(LED_PIN, HIGH); // LED on if needs sync
  }

}

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    if(pctime > DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
      setTime(pctime); // Sync Arduino clock to the time received on the serial port
    }
  }
}

time_t requestSync() {
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}
