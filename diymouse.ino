//3D printed Mouse

#include "PAN3101.h"
#include <FastLED.h>
#include "Rotary.h"
#include <HID-Project.h>

#define SCLK 2                            // Serial clock pin on the Arduino
#define SDIO 3                            // Serial data (I/O) pin on the Arduino
#define PIN_ENCODER_A 7
#define PIN_ENCODER_B 8

CRGB leds[4];
PAN3101 Optical1 = PAN3101(SCLK, SDIO);
Rotary rotary = Rotary(PIN_ENCODER_A, PIN_ENCODER_B);

const int pinLeftClick = 4;
const int pinRightClick = 5;
const int pinMiddleClick = 6;
const int ledPin = 9;
const char keyMap[3]={MOUSE_LEFT,MOUSE_RIGHT,MOUSE_MIDDLE};
int state[3]={HIGH,HIGH,HIGH};
int last[3]={0,0,0};
char lastQual = 0;
char qTime=0;

void setAllLeds(CRGB val){
  for(int i=0; i<4; i++){
    leds[i] = val;
  }
  FastLED.show();
}
void setup() {
  FastLED.addLeds<WS2812B, ledPin,GRB>(leds, 4);
  Optical1.begin();
  setAllLeds(CRGB(255,255,255));
  for(int i=4;i<=8;i++){
    pinMode(i, INPUT_PULLUP);
  }
  Mouse.begin();

}

void qualToLed(){
  int quality = Optical1.readRegister(0x19);
       setAllLeds(CHSV((quality+lastQual)/2,255,255));
       lastQual=quality;
}

void loop() {
  Optical1.updateStatus();
  
  int enc=0;
  int dX=0;
  int dY=0;
  if(qTime++ % 128 == 0){
       qualToLed();
     }
  if(Optical1.motion()){
     if(qTime % 16){
      qualToLed();
     }
     dX=-Optical1.dx();
     dY=Optical1.dy();
     
     
  }
  
  unsigned char rotRes = rotary.process();
  if(rotRes==DIR_CW)
    enc = -1;
  else if(rotRes==DIR_CCW)
    enc = 1;
    
  for(int i=0;i<3;i++){
    int cur=digitalRead(i+pinLeftClick);
    int mtime = micros();
    if(cur!=state[i]&&((mtime-last[i])>5000)){  
      if(cur==LOW){
          Mouse.press(keyMap[i]);
      }else{
          Mouse.release(keyMap[i]);}
      state[i]=cur;
      last[i]=mtime;     
    }
  }
  
  if(dX!=0 || dY!=0 || enc!=0){
    Mouse.move(dX, dY, enc);
    
  }

}
