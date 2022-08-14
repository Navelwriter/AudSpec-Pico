#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>



#define Button1 0
//define button_isr




/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
*/
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);



void setup(void) {
  u8g2.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Button1, INPUT);
  Serial.begin(9600);

 

  
}

void loop(void) {
  //scroll text to the right and left with a delay of 1 second between each step (1000 ms) and a speed of 1 pixel per second (1 pixel per 1000 ms) 
  u8g2.setFont(u8g2_font_6x10_mf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  //find character width of "Hello World"
  uint8_t char_width = u8g2.getStrWidth("Hello World");
  //scroll text to the right until the right border is reached by the end of the string
  for (int i = 0; i < 128 - char_width; i++) {
    u8g2.drawStr(i, 10, "Hello World");
    u8g2.sendBuffer();
    delay(10);
    u8g2.clearBuffer();

  }
  //scroll text to the left until the left border is reached by the end of the string
  for(int i = 127 - char_width; i >= 0; i--) {
    u8g2.drawStr(i, 10, "Hello World");
    u8g2.sendBuffer();
    delay(10);
  }
  //Print to serial monitor the isr button state
  Serial.println(digitalRead(Button1)); 
  
 

  
}

//Interrupt Service Routine for Button1 with debouncing in raspberry pi pico board
void ISR_Button1() {
  //check if the button is pressed
  if (digitalRead(Button1) == HIGH) {
    //turn on the led
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    //turn off the led
    digitalWrite(LED_BUILTIN, LOW);
  }
}
