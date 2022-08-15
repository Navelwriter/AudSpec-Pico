#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <adc.h>
#include <Bounce2.h>

#define Button1 17
#define ADC2_GPIO 34
int ledState = LOW;
int val = 0;
int mem = 0;
Bounce b = Bounce(); // create a Bounce object

// This array converts a number 0-9 to a bit pattern to send to the GPIOs
int bits[21] = {
    0x0,     // 0
    0x1,     // 1
    0x3,     // 2
    0x7,     // 3
    0xF,     // 4
    0x1F,    // 5
    0x3F,    // 6
    0x7F,    // 7
    0xFF,    // 8
    0x1FF,   // 9
    0x3FF,   // 10
    0x7FF,   // 11
    0xFFF,   // 12
    0x1FFF,  // 13
    0x3FFF,  // 14
    0x7FFF,  // 15
    0xFFFF,  // 16
    0x1FFFF, // 17
    0x3FFFF, // 18
    0x7FFFF, // 19
    0xFFFFF, // 20

};

static char event_str[128];
//prototype function for button_isr
void button_ISR();
/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.

*/
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

int map(int s, int a1, int a2, int b1, int b2) {
  return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
}

void setup() {
  u8g2.begin();
  adc_init();
  adc_gpio_init(ADC2_GPIO);
  adc_select_input(2); // select ADC input 2
  
  pinMode(LED_BUILTIN, OUTPUT); // set Built_in_LED as output
  digitalWrite(LED_BUILTIN, !ledState); // set Built_in_LED to LOW

  pinMode(Button1, INPUT_PULLUP); // set Button1 as input with pullup
  b.attach(Button1); // attach Button1 to the Bounce object
  b.interval(10); // set the bounce interval to 10ms
  Serial.begin(9600); 
  //print hello message on the serial monitor
  Serial.println("Hello World!");
  
}

// create a u8g2 rectangle class with functions to draw and fill it
class u8g2_rect_t {
public:
  u8g2_rect_t(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
  }
  void draw(U8G2 *u8g2) {
    u8g2->drawFrame(this->x, this->y, this->w, this->h);
  }
  void fill(U8G2 *u8g2) {
    u8g2->drawBox(this->x, this->y, this->w, this->h);
  }
  // move the rectangle by dx, dy
  void move(int dx, int dy) {
    this->x += dx;
    this->y += dy;
  }

private:
  int x;
  int y;
  int w;
  int h;
};

void loop() {
  u8g2.clearBuffer();
  uint16_t result = adc_read();
  // map it between 0 and 15
  val = map(result, 0, 0xfff, 0, 20);
  if (val != mem) {
    mem = val;
  }
  uint8_t char_width = u8g2.getStrWidth("Hello World");
  sprintf(event_str, "ADC: %d, %d", result, val);
  u8g2.setFont(u8g2_font_6x10_mf);
  u8g2.drawStr(u8g2.getDisplayWidth() / 2 - char_width / 2, u8g2.getDisplayHeight() / 2, event_str); // draw the string at the center of the screen

  u8g2_rect_t rect(u8g2.getDisplayWidth() - u8g2.getDisplayWidth() / 10, 0, u8g2.getDisplayWidth() / 12, u8g2.getDisplayHeight() / 5); // create a new narrow rectangle on the top right
  rect.move(0, val * 2.5); // move the rectangle using the potentiometer
  rect.fill(&u8g2);
  rect.draw(&u8g2);
  u8g2.sendBuffer();

  b.update(); // update the Button object
  Serial.println(b.read());
  if(b.fell()){ // if the button was pressed
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState); // set Built_in_LED to the opposite state
  }
}

