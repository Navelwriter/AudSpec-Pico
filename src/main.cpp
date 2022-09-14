#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <Bounce2.h>
#include <multicore.h>
#include "arduinoFFT.h"

#define Button1 17
#define ADC2_GPIO 28
#define R_IN 26 //Right audio input
#define L_IN 27 //Left audio input
#define Wave_X1 62 //Origin Left X
#define Wave_X2 65 //Origin Right X    
#define Wave_Y1 16 //Bottom edge of waveform   
#define Wave_Y2 55 //Lower edge of waveform (-50db)  

int ledState = LOW;
int val = 0;
int mem = 0;
static char event_str[128]; // General purpose string for printing to the screen
Bounce b = Bounce(); // create a Bounce object

//enum 4 states for the average of the waveform named "state"
enum waveState{
  WAVE_ON,
  WAVE_OFF,
};
enum waveState state = WAVE_OFF;

void button_ISR();
void showWave();
void showSpect();
int barHeight(double value);

arduinoFFT FFT = arduinoFFT(); // create an instance of the FFT class
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

const uint16_t samples = 128; // size of the FFT buffer
double vReal_R[samples]; // real part of the FFT output       
double vImag_R[samples]; // imaginary part of the FFT output 
double vReal_L[samples];
double vImag_L[samples];
int16_t wave_R[samples]; // waveform data for the FFT input      
int16_t wave_L[samples];

int map(int s, int a1, int a2, int b1, int b2) { // map a value from one range to another
  return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
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

void setup() {
  u8g2.begin();
  //initialize the second adc channel
  // adc_gpio_init(ADC2_GPIO);
  // adc_select_input(2); // select ADC input 2
  analogReadResolution(12); // set the ADC resolution to 12 bit

  pinMode(LED_BUILTIN, OUTPUT); // set Built_in_LED as output
  digitalWrite(LED_BUILTIN, !ledState); // set Built_in_LED to LOW
  pinMode(Button1, INPUT_PULLUP); // set Button1 as input with pullup
  b.attach(Button1); // attach Button1 to the Bounce object
  b.interval(10); // set the bounce interval to 10ms
  Serial.begin(9600); 
  u8g2.setFont(u8g2_font_6x10_mf);
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();       
  u8g2.clearBuffer();
  u8g2.drawStr( 0, 0, "Start AudSpec");
  u8g2.sendBuffer();
  delay(500);

}

void loop() {
  u8g2.clearBuffer();
  b.update(); // update the Button object
  Serial.println(b.read());
  if(b.fell()){ // if the button was pressed
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState); // set Built_in_LED to the opposite state
  }
  uint16_t result = analogRead(ADC2_GPIO); //read the potentiometer adc value in the ADC2
  val = map(result, 0, 0xfff, 0, 20); // map it between 0 and 15
  if (val != mem) {
    mem = val;
  } 
  showWave();
  showSpect();
  u8g2.sendBuffer();

}

void setup1(){ 
}

void loop1(){
  for (int i = 0; i < samples; i++) { 
    wave_R[i] = analogRead(R_IN); // read the waveform data from the ADC
    wave_L[i] = analogRead(L_IN);
    delayMicroseconds(21); // delay for 21 microseconds to get the next sample
  }       

  
  for (int i = 0; i < samples; i++) { // perform the FFT on the waveform data
    vReal_R[i] = (wave_R[i] - 2048) * 3.3 / 4096.0; // convert the waveform data to a voltage
    vReal_L[i] = (wave_L[i] - 2048) * 3.3 / 4096.0;
    vImag_R[i] = 0; // set the imaginary part to 0
    vImag_L[i] = 0;
  }

  FFT.Windowing(vReal_R, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); // apply the windowing function
  FFT.Windowing(vReal_L, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); 
 
  FFT.Compute(vReal_R, vImag_R, samples, FFT_FORWARD); // compute the FFT
  FFT.Compute(vReal_L, vImag_L, samples, FFT_FORWARD);
 
  FFT.ComplexToMagnitude(vReal_R, vImag_R, samples); // compute the magnitude of the FFT output
  FFT.ComplexToMagnitude(vReal_L, vImag_L, samples);

}


void showWave() { // draw the waveform data on the screen
  for (int i = 0; i < 52; i++) {
    u8g2.drawLine(Wave_X2 + i, Wave_Y1 - (wave_R[i * 2]) / 256, Wave_X2 + i + 1, 16 - (wave_R[i * 2 + 1] / 256)); // draw the waveform data for the right channel
    u8g2.drawLine(Wave_X1 - i, Wave_Y1 - (wave_L[i * 2]) / 256, Wave_X1 - i - 1, 16 - (wave_L[i * 2 + 1] / 256)); // draw the waveform data for the left channel
  }
}

void showSpect() { // draw the spectrum data on the screen        
  int mag;
  static int peak_R[64];     
  static int peak_L[64];
  int average = 0;
  
 
  for (int xi = 1; xi < 60; xi++) {  // loop through the spectrum data    
    mag = barHeight(vReal_R[xi]); // get the magnitude of the current frequency bin
    u8g2.drawVLine(xi + Wave_X2, Wave_Y2 - mag, mag); // draw the magnitude of the current frequency bin         
    u8g2.drawVLine(xi + Wave_X2, Wave_Y2 - peak_R[xi], 1);  // draw the peak of the current frequency bin
    if (peak_R[xi] < mag) {  // if the current frequency bin is greater than the peak of the current frequency bin                         
      peak_R[xi] = mag; // set the peak of the current frequency bin to the magnitude of the current frequency bin                              
    }
    if (peak_R[xi] > 0) { // if the peak of the current frequency bin is greater than 0 
      peak_R[xi] --; // decrement the peak of the current frequency bin                               
    }
    average += mag; // add the magnitude of the current frequency bin to the average
 
    mag = barHeight(vReal_L[xi]);       
    u8g2.drawVLine(Wave_X1 - xi, Wave_Y2 - mag, mag);          
    u8g2.drawVLine(Wave_X1 - xi, Wave_Y2 - peak_L[xi], 1); 
    if (peak_L[xi] < mag) {                          
      peak_L[xi] = mag;                              
    }
    if (peak_L[xi] > 0) {
      peak_L[xi] --;                               
    }
  }
  average = average / 60; // get the average of the spectrum data (just the right channel)
  
  sprintf(event_str, "Average: %d, %d", average, val); //set character buffer to average as well as potentiometer value
  u8g2.setFont(u8g2_font_6x10_mf); //print character buffer to middle of screen
  u8g2.drawStr(u8g2.getDisplayWidth() / 2 - u8g2.getStrWidth(event_str) / 2, u8g2.getDisplayHeight() / 2, event_str);

  if (average > 6) {  //if average is above 6 set the enumerated state to WAVE_ON
    state = WAVE_ON;
  }
  else{
    state = WAVE_OFF;
  }

}
 
int barHeight(double mag) {             
  float fy; 
  int y; 
  fy = 14.0 * (log10(mag) + 1.5); // get the magnitude of the current frequency bin in dB      
  y = fy;
  y = constrain(y, 0, 56); // constrain the magnitude of the current frequency bin to the range 0 to 56
  if(state == WAVE_OFF){ //Essentially a filter to reduce the amount of noise in the spectrum data
    if(y <= 20 && y >= 10) { // if the magnitude of the current frequency bin is between -20 and -15 dB
    y = y / 2;
    }
    else if(y <= 10 && y >= 5) { // if the magnitude of the current frequency bin is between -10 and -5 dB
      y = y / 1.75;
    }
    else if(y <= 5 && y >= 0) { // if the magnitude of the current frequency bin is between -5 and 0 dB
     y = 0;
    }
  }  
  return y;
}


