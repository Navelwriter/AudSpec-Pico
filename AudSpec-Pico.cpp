#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <adc.h>


#define Button1 0
#define ADC2_GPIO 34


// This array converts a number 0-9 to a bit pattern to send to the GPIOs
int bits[11] = {
  0x0,  // 0
  0x1,  // 1
  0x3,  // 2
  0x7,  // 3
  0xF,  // 4
  0x1F,  // 5
  0x3F,  // 6
  0x7F,  // 7
  0xFF,  // 8
  0x1FF,  // 9
  0x3FF,  // 10
};

static char event_str[128];

/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
*/
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int map(int s, int a1, int a2, int b1, int b2) {
  return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
}

void setup() {
  u8g2.begin();
  adc_init();
  adc_gpio_init(ADC2_GPIO);
  adc_select_input(2); // select ADC input 2
  Serial.begin(9600);

  int val = 0;
  int mem = 0;
}

void loop() {
  uint16_t result = adc_read();
  if(result >= 0 && result < 1024) { // if result is between 0 and 1024 (10 bit ADC) then map it to 0 and 100 and display it on the screen 
    int val = map(result, 0, 1024, 0, 10);
    if(val != mem) {
      mem = val;
      sprintf(event_str, "ADC: %d", val);
      u8g2.firstPage();
      do {
        u8g2.setFont(u8g2_font_6x10_mf); 
        u8g2.drawStr(0, 10, event_str); // draw the string
      } while(u8g2.nextPage());
    }
  }
  // //scroll text to the right and left with a delay of 1 second between each step (1000 ms) and a speed of 1 pixel per second (1 pixel per 1000 ms) 
  // u8g2.setFont(u8g2_font_6x10_mf);
  // u8g2.setFontRefHeightExtendedText();
  // u8g2.setDrawColor(1);
  // u8g2.setFontPosTop();
  // //find character width of "Hello World"
  // uint8_t char_width = u8g2.getStrWidth("Hello World");
  // //scroll text to the right until the right border is reached by the end of the string
  // for (int i = 0; i < 128 - char_width; i++) {
  //   u8g2.drawStr(i, 10, "Hello World");
  //   u8g2.sendBuffer();
  //   delay(10);
  //   u8g2.clearBuffer();

  // }
  // //scroll text to the left until the left border is reached by the end of the string
  // for(int i = 127 - char_width; i >= 0; i--) {
  //   u8g2.drawStr(i, 10, "Hello World");
  //   u8g2.sendBuffer();
  //   delay(10);
  // }

  
 

  
}
