////                                  ////
///         Library Imports          ///
//                                  //
#include       <Arduino.h>
#include       <U8g2lib.h>
#include       <Bounce2.h>
#include  <ESP32Encoder.h>
#include           <SPI.h>
//#include        <Wire.h>
#include          <math.h>
#include          <vector>
#include        <SPIFFS.h>
#include          <random>
#include         "anims.h"

////                                  ////
///     Variable Initialization      ///
//                                  //
//Pin Definitions
#define SENCODER_CLK     2
#define SENCODER_DT      3
#define SENCODER_CPR    94

#define LENCODER_CLK    18
#define LENCODER_DT     19
#define LENCODER_CPR    94

#define AButtonPin      42
#define BButtonPin      41
#define UPButtonPin     39
#define DOWNButtonPin   40
#define LEFTButtonPin   20
#define RIGHTButtonPin  21
#define powerButtonPin   0

//Input Setup
bool breakLoop = false;
unsigned long encoderDelay = 10;

ESP32Encoder sEncoder;
ESP32Encoder lEncoder;

Bounce aButton = Bounce();
Bounce bButton = Bounce();

int sEncoder_change = 0;
int lEncoder_change = 0;

//Sleep Mode
volatile bool sleeping = false;

//FPS
bool          SHOW_FPS    = false;          // < SHOW FPS ON SCREEN
unsigned int  LIMITFPSTO  = 60;             // < LIMIT SCREEN FPS
unsigned long millisFPS   = 0;
unsigned long lastMillis  = 0;
unsigned int  frameCount  = 0;
unsigned int  fps         = 0;

//Animations
volatile int  animFrames[]     = {0,1,0,0,0,0,0,0,0}; //pageFlipAnim, bigGear2, spinDots, settingsIcon, animCalendar, animTasks, gamesIcon, crane, party
unsigned long lastMillis30FPS  = 0;
bool          step30FPS        = false;
int           step1FPSInt      = 0;
bool          step1FPS         = false;
bool          halfFrame        = true;
bool          quarterFrame     = true;

//Calendar App
bool exitCalendarApp = false;
#define MAX_CALENDAR_EVENTS 99
struct CalendarEvent {unsigned long unixTime;String eventName;};
CalendarEvent calendarEvents[MAX_CALENDAR_EVENTS];

//Settings App
bool exitSettingsApp = false;
enum SettingsAppState {SETTINGS_INTRO, SETTINGS_HOME,BRIGHTNESS};
SettingsAppState settingsCurrentState = SETTINGS_INTRO;
bool breakCraneIntro = false;
int settingsEncoderCount = 0;

//Games App
bool exitGamesApp = false;

//OLED
U8G2_SH1107_PIMORONI_128X128_F_4W_HW_SPI u8g2(U8G2_R0, 10, 9 , 8);

////                                  ////
///     Setup & Common Functions     ///
//                                  //
void IRAM_ATTR powerButtonInterrupt() {
  if (sleeping) {
    sleeping = false;
  }
  else {
    sleeping = true;
  }
  
}

int getRandomNumber(int min, int max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(min, max);
  return dis(gen);
}

void pageFlipAnim() {
  int frame = 0;
  while (frame<12) {
    u8g2.setDrawColor(0);
    switch (frame+1) {
      case 1:
        u8g2.drawBox(124,123,4,5);
        break;
      case 2:
        u8g2.drawBox(118,119,10,9);
        break;
      case 3:
        //u8g2.drawTriangle(106,127, 112,113, 127,114);
        //u8g2.drawTriangle(106,127, 128,128, 127,114);
        drawQuad(106,127,112,113,127,114,128,128);
        break;
      case 4:
        drawQuad(86,127,100,101,127,108,128,128);
        break;
      case 5:
        drawQuad(64,127,92,84,127,99,128,128);
        break;
      case 6:
        drawQuad(48,127,92,67,127,84,128,128);
        break;
      case 7:
        drawQuad(24,127,93,51,128,74,128,128);
        break;
      case 8:
        drawQuad(5,127,94,34,127,63,128,128);
        break;
      case 9:
        drawQuad(0,110,100,20,127,51,0,118);
        u8g2.drawTriangle(0,118,128,51,128,118);
        u8g2.drawBox(0,118,128,10);
        break;
      case 10:
        drawQuad(0,75,107,5,127,44,0,94);
        u8g2.drawTriangle(0,93,128,44,128,93);
        u8g2.drawBox(0,93,127,40);
        break;
      case 11:
        drawQuad(0,34,121,0,127,44,0,94);
        u8g2.drawTriangle(0,62,128,31,128,62);
        u8g2.drawBox(0,62,128,50);
        break;
      case 12:
        u8g2.clearBuffer();
        break;
    }
    u8g2.setDrawColor(1);
    u8g2.drawXBMP(0, 0, 128, 128, pageFlip_allArray[frame]);
    u8g2.sendBuffer();
    frame++;
    delay(20);
  }
  animFrames[0] = 0;
  u8g2.clearBuffer();
}

void drawQuad(int ax, int ay, int bx, int by, int cx, int cy, int dx, int dy) {
  u8g2.drawTriangle(ax,ay, bx,by, cx,cy);
  u8g2.drawTriangle(ax,ay, dx,dy, cx,cy);
}

void updateAnims() {
  unsigned long currentMillis30FPS = millis();
  if (step30FPS) {step30FPS = false;}
  if (currentMillis30FPS - lastMillis30FPS >= 33) {
    step1FPSInt++;
    if (step1FPSInt > 30) {
      step1FPS = true;
      step1FPSInt = 0;
    }
    else {step1FPS = false;}

    // Save the current time
    lastMillis30FPS = currentMillis30FPS;
    step30FPS = true;
  }

  if (animFrames[0] == 1) { //Page Flip
    u8g2.setBitmapMode(1);
    pageFlipAnim();
  }
  if (animFrames[1] != 0) { //Big Gear 2
    u8g2.drawXBMP(77,4,50,120,bigGear2allArray[animFrames[1]-1]);
  }
}

void fpsCounter() {
  if (SHOW_FPS){
    frameCount++;
    if (millis() - lastMillis >= 1000) {            //FPS COUNTER
      fps = frameCount;
      Serial.print("FPS: "); Serial.println(fps);
      frameCount = 0;lastMillis = millis();
    }
    u8g2.drawStr(0,128,("FPS: "+String(fps)).c_str()); //Show current FPS
  }
}

void largeClock(int type, int time) {

  int digit1 = time / 1000;       // Thousands place
  int digit2 = (time / 100) % 10; // Hundreds place
  int digit3 = (time / 10) % 10;  // Tens place
  int digit4 = time % 10;         // Ones place

  u8g2.clearBuffer();
  u8g2.setBitmapMode(1);
  if (type == 0) {
    u8g2.drawXBMP( 0, 32, 36, 64, Solid_Numbers_allArray[digit1]);
    u8g2.drawXBMP(32, 32, 36, 64, Solid_Numbers_allArray[digit2]);
    u8g2.drawXBMP(64, 32, 36, 64, Solid_Numbers_allArray[digit3]);
    u8g2.drawXBMP(92, 32, 36, 64, Solid_Numbers_allArray[digit4]);
  }
  else if (type == 1) {
    u8g2.drawXBMP( 0, 32, 36, 64, Checkered_Numbers_allArray[digit1]);
    u8g2.drawXBMP(32, 32, 36, 64, Checkered_Numbers_allArray[digit2]);
    u8g2.drawXBMP(64, 32, 36, 64, Checkered_Numbers_allArray[digit3]);
    u8g2.drawXBMP(92, 32, 36, 64, Checkered_Numbers_allArray[digit4]);
  }

  u8g2.sendBuffer();
}

void displayFrame() {
  if (millis() - millisFPS >= int(1000/LIMITFPSTO)) {
    millisFPS = millis();
    fpsCounter();
    u8g2.sendBuffer();
    u8g2.clearBuffer();
  }
}

void setup(void) {
  //Serial.begin(9600);

  pinMode(AButtonPin, INPUT_PULLUP);
  pinMode(BButtonPin, INPUT_PULLUP);
  pinMode(powerButtonPin, INPUT_PULLUP);

  aButton.attach(AButtonPin);
  bButton.attach(BButtonPin);
  attachInterrupt(digitalPinToInterrupt(powerButtonPin), powerButtonInterrupt, FALLING);

  aButton.interval(5);
  bButton.interval(5);

  sEncoder.attachFullQuad(SENCODER_CLK, SENCODER_DT);
  lEncoder.attachFullQuad(LENCODER_CLK, LENCODER_DT);
  sEncoder.setCount(0);
  lEncoder.setCount(0);

  u8g2.begin();  // begin the u8g2 library
  u8g2.setContrast(255); // set display contrast/brightness
  u8g2.setBitmapMode(1);

  randomSeed(analogRead(0));
}

////                                  ////
///            Void  Loop            ///
//                                  //
void loop() { // main Arduino loop
  homeLoop();
}