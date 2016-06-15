int redPin = 9;
int grnPin = 10;
int bluPin = 11;

void setup() {
  Serial.println("Setup");
  pinMode(redPin, OUTPUT); 
  pinMode(grnPin, OUTPUT); 
  pinMode(bluPin, OUTPUT); 

}

void loop() {
  Serial.println("Loop");
  
  fadeIn(redPin);
  fadeIn(grnPin);
  fadeIn(bluPin);
  delay(100);
  fadeOut(redPin);
  fadeOut(grnPin);
  fadeOut(bluPin);
}


void fadeIn(int pin) {
  for(int r = 0; r < 0xFF; r++) {
    analogWrite(pin, r);
    delay(4);
  }
}

void fadeOut(int pin) {
  for(int r = 0xFF; r > 0; r--) {
    analogWrite(pin, r);
    delay(4);
  }
}

