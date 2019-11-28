//////////////////////////////////////////////////////////////////////////////////////////////
// Arduino code for using a servo to automate manual instruments (e.g. a digital camera)
//  and a user interface through an LDC+buttons shield
// Author: Gustavo Pereyra Irujo - pereyrairujo.gustavo@conicet.gov.ar
// Free & open source software. Licensed under GPLv3 https://www.gnu.org/licenses/gpl-3.0.html

#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#include <Servo.h> 
Servo myservo;

#include <EEPROM.h>

// define some values used by the LCD panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
int backlightDelay=0;

// initial values which can be overridden by values stored in EEPROM memory
int interval    = 7;    
int initial_pos = 72;
int final_pos   = 90;

char OK="K";
char isOK="-";

int action = 0;
#define actionMENU  0
#define actionSERVO 1
int menu_option = 0;

int start_time   = 0;
int elapsed_time = 0;
int servo_count  = 0;
int previous_servo_count  = 0;

int read_LCD_buttons()
{
 adc_key_in = analogRead(0); 
 if (adc_key_in > 1000) return btnNONE;
 if (adc_key_in < 80)   return btnRIGHT; 
 if (adc_key_in < 195)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 555)  return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;   
 return btnNONE;  // when all others fail, return this
}

void LCDbacklight(boolean lightON) {   // turning the backlight on the LCD panel on or off http://forum.arduino.cc/index.php?topic=96747.15
  if (lightON) pinMode(10,INPUT);      // setting pin 10 to input turns backlight on
  else {
    pinMode(10, OUTPUT);
    digitalWrite(10,LOW); // turn off backlight
  }
}

void move_servo()           // move the servo to the final position and back to the initial position
{
  // move servo to final position
  for (int i = initial_pos; i <= final_pos; i++) {
   myservo.write(i);
   delay(20);
  }
  beep(200);
  // move servo back to initial position
  for (int i = final_pos-1; i > initial_pos; i--) {
   myservo.write(i);
   delay(10);
  } 
}

void beep(unsigned char delayms){
  analogWrite(3, 50);      // Almost any value can be used except 0 and 255
                           // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  analogWrite(3, 0);       // 0 turns it off
  delay(delayms);          // wait for a delayms ms   
}

void eepromWrite()             // write parameter values to persistant EEPROM memory
{
  int eeAddress = 0;
  EEPROM.put(eeAddress, OK);    // write "K" at the beginning for checking later
  eeAddress += sizeof(char);
  EEPROM.put(eeAddress, interval);
  eeAddress += sizeof(int);
  EEPROM.put(eeAddress, initial_pos);
  eeAddress += sizeof(int);
  EEPROM.put(eeAddress, final_pos);
}

void eepromRead()             // read previous parameter values from persistant EEPROM memory
{
  int eeAddress = 0;
  EEPROM.get(eeAddress, isOK);    // check if values have been previously stored by this program
  if(isOK==OK) {
    eeAddress += sizeof(char);
    EEPROM.get(eeAddress, interval);
    eeAddress += sizeof(int);
    EEPROM.get(eeAddress, initial_pos);
    eeAddress += sizeof(int);
    EEPROM.get(eeAddress, final_pos);
  }
}

void menuDisplay() {          // show menu items in the LCD display
    switch (menu_option)
    {
      case 0:
        lcd.setCursor(0,0);
        lcd.print("Interval        ");
        lcd.setCursor(0,1);
        if (interval<60) {
          lcd.print(interval);
          lcd.print(" sec            ");
        }
        else {
          lcd.print(interval/60);
          lcd.print(" min            ");
        }
        break;
      case 1:
        lcd.setCursor(0,0);
        lcd.print("Initial position");
        lcd.setCursor(0,1);
        lcd.print(initial_pos);
        lcd.print("                ");
        break;
      case 2:
        lcd.setCursor(0,0);
        lcd.print("Final position  ");
        lcd.setCursor(0,1);
        lcd.print(final_pos);
        lcd.print("                ");
        break;
    }  
}

void servoDisplay() {          // show servo data in the LCD display
    // first line
    lcd.setCursor(0,0);
    lcd.print("Int.(sec): ");
    lcd.print(interval);
    lcd.print("        ");
    // second line
    lcd.setCursor(0,1);
    lcd.print("N: ");
    lcd.print(servo_count);
    lcd.print("      ");
    lcd.setCursor(11,1);
    lcd.print(elapsed_time % interval);
    lcd.print("        ");  
}

void setup()
{
  eepromRead();                  // read previous parameter values from persistant EEPROM memory
  pinMode(3, OUTPUT);            // buzzer
  lcd.begin(16, 2);              // start the library
  myservo.attach(2);
  myservo.write(initial_pos);    // move servo to initial position
}
 
void loop()
{
  if (action==actionMENU) {
    
    menuDisplay();

    lcd_key = read_LCD_buttons();  // read the buttons
    if (lcd_key!=btnNONE) {
         LCDbacklight(true); // turn on backlight if any key is pressed
         backlightDelay=0;
         switch (lcd_key) // depending on which button was pushed, perform an action
         {
           case btnRIGHT:                         // increase value
             if (menu_option==0) {
                if (interval>=3600 && interval < 28800) interval=interval+600;
                else if (interval>=600) interval=interval+300;
                else if (interval>=60) interval=interval+30;
                else interval=interval+1;
             }
             else if (menu_option==1){
                initial_pos=initial_pos+1;
                myservo.write(initial_pos);
             }
             else if (menu_option==2){
                final_pos=final_pos+1;
                move_servo();
             }
             break;
           case btnLEFT:                          // decrease value
             if (menu_option==0) {
                if (interval>=4200) interval=interval-600;
                else if (interval>=900) interval=interval-300;
                else if (interval>=90) interval=interval-30;
                else if (interval>5) interval=interval-1;
             }
             else if (menu_option==1){
                initial_pos=initial_pos-1;
                myservo.write(initial_pos);
             }
             else if (menu_option==2){
                final_pos=final_pos-1;
                move_servo();
             }
             break;
           case btnUP:                            // go to previous menu option
             if (menu_option>0) menu_option=menu_option-1;
             break;
           case btnDOWN:                          // go to next menu option
             if (menu_option<2) menu_option=menu_option+1;
             break;
           case btnSELECT:                        // start servo
             action=actionSERVO;
             interval    = interval;
             initial_pos = initial_pos;
             final_pos   = final_pos;
             eepromWrite(); // write values to persistant EEPROM memory
             start_time  = millis()/1000;
             break;
        }
        delay(200);
    }
    else if(backlightDelay<100) backlightDelay++;
    else LCDbacklight(false);
  }

  if (action==actionSERVO) {
    
    elapsed_time = (millis()/1000)-start_time;
    previous_servo_count = servo_count;
    servo_count = elapsed_time / interval + 1;

    servoDisplay();

    if (servo_count!=previous_servo_count) move_servo();

    lcd_key = read_LCD_buttons();  // read the buttons
    if (lcd_key!=btnNONE) {        // turn on backlight if any key is pressed
      LCDbacklight(true);
      backlightDelay=0;
    }
    else if(backlightDelay<100) backlightDelay++;
    else LCDbacklight(false);
  }
  
  delay(100);
}

