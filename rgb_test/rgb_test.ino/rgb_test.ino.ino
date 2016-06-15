
#define RED_PIN 23
#define GRN_PIN 22
#define BLU_PIN 21

void setup() {
  // put your setup code here, to run once:
  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  pinMode(BLU_PIN, OUTPUT);  

}

int i = 0;

void loop() {
  // put your main code here, to run repeatedly:

  /*
  switch(i % 3) {
    case 0:
      rgb(0xFF, 0, 0);
      break;
    case 1:
      rgb(0, 0xFF, 0);
      break;
    case 2:
      rgb(0, 0, 0xFF);
      break;
  }
  i++;  
  delay(500);
  */
  
  
  for (int r = 0; r < 0xFF; r++) {    
    if (r == 0) {
      Serial.println("Red starting over\n");
    }    
    for (int g = 0; g < 0xFF; g++) {      
      for (int b = 0; b < 0xFF; b++) {
        rgb(r, g, b);
      }
    }

  }
  
}

void rgb(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(RED_PIN, r);
  analogWrite(GRN_PIN, g);
  analogWrite(BLU_PIN, b);
}

