// Sea Bees Virtual pet
// Pro Trinket 3V, 12MHz
// Adafruit 1.8" Color TFT Display (128px x 128px)
//-----------------------------------------------------------

// Includes
//-----------------------------------------------------------
#include "color.h"
#include "graphics.h"
#include "bees.h"

// Hardware setup I dont understand
//-----------------------------------------------------------
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <EEPROM.h>



// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    9  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     8

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);



// Global Variables
//-----------------------------------------------------------
#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )

//globals
int test = 0;

//buttons
const int buttonPin1 = 5;
const int buttonPin2 = 6;
const int buttonPin3 = 12;

bool canPress[3] = {true,true,true};
bool pressedBtns[3] = {false,false,false};
bool btnstates[3];

//animation
int frame = 0;
const long FRAME_RATE = 500; 
const int MAX_FRAMES = 3;
unsigned long previousMillis = 0; 

//application states
int state = 0;
// 0 - start screen
// 1 - plant view
// 2 - bee view
// 3 - bee selection
// 4 - admin

void setup()
{
  // GREEENTAB is for my particular screen
  tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
  //  tft.setRotation(2);

  tft.fillScreen(COLOR_BG); //fill TFT with black color.

  //  sets inputs
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);

  //  uint8_t read
//  test = EEPROM.read(0);
}


/**
 * Main application loop
 */
void loop() {
  btnPresses();
  updateState();  
  drawCurrentState();
  updateFrames();
}


/**
 * Routes to routines for drawing the current state
 */
void drawCurrentState() {
  if (state == 4) {
    drawDevState();
  } else if (state == 2) {
    drawBeeState();
  }
}

/** 
 * Progresses State based on button presses and previous states 
 */
void updateState() {  
  int prevState = state;

  if (btnstates[0] && btnstates[2]) {
    state = 4;
  } else {
    state = 2;
  }

  if (state != prevState) {
    tft.fillScreen(COLOR_BG);
  }
}

/**
 * Draws various dev states to the screen, 
 *  helpful for debugging
 */
void drawDevState() {
  //  Button test output
  tft.setCursor(0,0);
  tft.setTextSize(4);
  tft.setTextColor(COLOR_BLUE,COLOR_BG);
  
  tft.setTextSize(1);

  if (pressedBtns[1]) {
    test++;
  }
  tft.setCursor(0,34);
  tft.print("press:");
  tft.print(test);
}

/**
 * Handles Drawing the Bee State
 */
void drawBeeState() {
  draw_matrix(leftArrow,1,15);
  draw_matrix(rightArrow,28,15);
  
  if (frame < 1) {
    print_bee(get_bee(1));
  } else if (frame < 2) {
    print_bee(get_bee(2));
  } else {
    print_bee(get_bee(3));
  }
}

/**
 * Updates current frame for animation timing by FRAME_RATE
 */
void updateFrames() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= FRAME_RATE) {
    previousMillis = currentMillis;
    
    frame ++;
    if (frame == MAX_FRAMES) {
      frame = 0;
    }
  }
}

/**
 * Handles settings states for putton presses
 *  Tracks btnstates (if held down) and pressedBtns (one press)
 */
void btnPresses() {
  pressedBtns[0] = false;
  pressedBtns[1] = false;
  pressedBtns[2] = false;
  
  btnstates[0] = digitalRead(buttonPin1);
  btnstates[1] = digitalRead(buttonPin2);
  btnstates[2] = digitalRead(buttonPin3);
  
  for (int i=0; i<3; i++) {
    if (btnstates[i] && canPress[i]) {
      canPress[i] = false;
      if (!pressedBtns[i]) {
        pressedBtns[i] = true;
      }
    } else if (!btnstates[i]) {
      pressedBtns[i] = false;
      canPress[i] = true;
    }
  }
  //  EEPROM.write(loc, val);
}

void print_bee(char *bee) {
  int xOffset = (32 - bee[1])/2;
  int yOffset = (32 - bee[0]/bee[1])/2;
  draw_matrix(bee, xOffset, yOffset);
  free(bee);
}

/**
 * Draws a grid of 4x4 rects to the screen from an int array
 *  of any length
 * @param {*int[]} img Array of pixel values, where the first
 *    digit is the number of pixels to draw, and the second is
 *    the length of each row of pixels
 * @param {int} x Offset x in 4x4 units to start draw
 * @param {int} y Offset y in 4x4 units to start draw
 */
void draw_matrix(char *img, int x, int y) {
  int cx_o = x*4;
  int cx = x*4;
  int cy = y*4;
  int px_cnt = img[0];
  int row = img[1];
  
  for (int i = 2; i < px_cnt + 2; i++) {
//    if (img[i] != 0) {
      tft.fillRect(cx, cy, 4, 4, colors[img[i]]);
//    }

    // increase row/col    
    cx += 4;
    if ((i-1)%row == 0) { 
      cy+=4;
      cx = cx_o;
    }
  }
}

