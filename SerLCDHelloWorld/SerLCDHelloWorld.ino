#define LCD Serial1

void setup()
{
  LCD.begin(9600);
  backlightOn();
}

void loop()
{  
  selectLineOne();
  LCD.print(millis());
  selectLineTwo();
  LCD.print(millis()/2);
  delay(100);
}

void selectLineOne(){  //puts the cursor at line 0 char 0.
   LCD.write(0xFE);   //command flag
   LCD.write(128);    //position
   delay(10);
}
void selectLineTwo(){  //puts the cursor at line 0 char 0.
   LCD.write(0xFE);   //command flag
   LCD.write(192);    //position
   delay(10);
}
void goTo(int position) { //position = line 1: 0-15, line 2: 16-31, 31+ defaults back to 0
if (position<16){ LCD.write(0xFE);   //command flag
              LCD.write((position+128));    //position
}else if (position<32){LCD.write(0xFE);   //command flag
              LCD.write((position+48+128));    //position 
} else { goTo(0); }
   delay(10);
}

void clearLCD(){
   LCD.write(0xFE);   //command flag
   LCD.write(0x01);   //clear command.
   delay(10);
}
void backlightOn(){  //turns on the backlight
    LCD.write(0x7C);   //command flag for backlight stuff
    LCD.write(157);    //light level.
   delay(10);
}
void backlightOff(){  //turns off the backlight
    LCD.write(0x7C);   //command flag for backlight stuff
    LCD.write(128);     //light level for off.
   delay(10);
}
void serCommand(){   //a general function to call the command flag for issuing all other commands   
  LCD.write(0xFE);
}
