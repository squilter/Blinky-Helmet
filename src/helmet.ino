#include <Adafruit_NeoPixel.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
    
#define NEOPIXELS 6
#define LED 7
#define TOUCH 10
#define PULSE 9

int fadeRate = 0;  
boolean wore = false;

// these variables are volatile because they are used during the interrupt service routine!
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, NEOPIXELS, NEO_GRB + NEO_KHZ800);
Adafruit_GPS GPS(&Serial1);

//@TODO on pulse rising edge, blink lights a bit with fadey thing

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(LED, OUTPUT);
  pinMode(TOUCH, INPUT);
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  for (int i=0; i < 255+150; i++) {
    if (i>0){
      setWhite(0,i);
      setWhite(7,i);
    }
    if (i>50){
      setWhite(1,i-50);
      setWhite(6,i-50);
    }
    if (i>100){
      setWhite(2,i-100);
      setWhite(5,i-100);
    }
    if (i>150){
      setWhite(3,i-150);
      setWhite(4,i-150);
    }
    delay(5);
    strip.show();
  }
  pinMode(LED,OUTPUT);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
}

void loop(){
  boolean wearing=digitalRead(TOUCH);
  if(!wore&&wearing){
    //fade from white to red
    for (int i=255; i >= 0; i--) {
      setAllRed(i);
      delay(1);
    }
  }
  if(wore&&!wearing){
    //fade from red to white
    for (int i=0; i <= 255; i++) {
      setAllRed(i);
      delay(1);
    }
  }
  if(wore&&wearing){//set color to gps speed. 0 knots is 255,0,0.  10 knots is 255,255,0. 20 knots is 255,255,255
    byte red=255;
    byte green=0;
    by te blue=0;
    if(GPS.fix){
      int speed = GPS.speed;
      green=(byte) constrain(speed*255.0/10,0,255);
      blue=(byte) constrain((speed-10)*255.0/10,0,255);
    }
    for (int i=0; i <= 7; i++){
      strip.setPixelColor(i,red,green,blue);
    }
  }
  wore=wearing;
  //!!!
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return;
  }
  //!!!
}

void setWhite(int pixel, int level){
  if(level>255){
    level=255;
  }
  strip.setPixelColor(pixel,level,level,level);
}

void setAllRed(int level){//white is 255, red is 0
  for (int i=0; i <= 7; i++){
    strip.setPixelColor(i,255,level,level);
  }
}

