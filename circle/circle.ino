
#include <Adafruit_NeoPixel.h>
#include <Time.h>

#define NEO_PIN 11
#define LED_PIN 13
#define LED_COUNT 24
#define MAX_BRIGHTNESS 255

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
uint32_t red = strip.Color(255, 0, 0);
uint32_t green = strip.Color(0, 255, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t magenta = strip.Color(255, 0, 255);
uint32_t off = strip.Color(0, 0, 0);


void setup() {
  strip.begin();
  strip.setBrightness(MAX_BRIGHTNESS/10);
  strip.show(); // Initialize all pixels to 'off'

  Serial.begin(9600);
  setSyncProvider(requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");
}

void loop() {

  if (Serial.available()) {
    processSyncMessage();
  }

  if (timeStatus()!= timeNotSet) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, off);
    }
    strip.setPixelColor(hour() * 2, red);
    strip.setPixelColor(minute() / 2.5, green);
    strip.setPixelColor(second() / 2.5, blue);
  }

  if (timeStatus() == timeSet) {
    digitalWrite(LED_PIN, HIGH); // LED on if synced
  } else {
    digitalWrite(LED_PIN, LOW);  // LED off if needs refresh
  }

  strip.show();
}

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
      setTime(pctime); // Sync Arduino clock to the time received on the serial port
    }
  }
}

time_t requestSync() {
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}
