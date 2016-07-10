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
const int buttonPin1 = 5;
const int buttonPin2 = 6;
const int buttonPin3 = 12;
int arrowLState = 0;
int actionState = 0;
int arrowRState = 0;
int test = 0;

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
  test = EEPROM.read(0);
}

int frame = 0;
void loop() {
  
  
  if (frame < 10) {
    print_bee(get_bee(1));
  } else if (frame < 20) {
    print_bee(get_bee(2));
  } else {
    print_bee(get_bee(3));
  }

  frame ++;
  if (frame == 30) {
    frame = 0;
  }

  //  Button test output
  tft.setCursor(0,0);
  tft.setTextSize(4);
  tft.setTextColor(COLOR_BLUE,COLOR_BG);
  arrowLState = digitalRead(buttonPin1);
  actionState = digitalRead(buttonPin2);
  arrowRState = digitalRead(buttonPin3);
  tft.print(arrowLState);
  tft.print(actionState);
  tft.print(arrowRState);
  tft.setTextSize(1);
  if (actionState) {
    test++;
    EEPROM.write(0, test);
  }
  tft.setCursor(0,34);
  tft.print("press:");
  tft.print(test);
}

void print_bee(int *bee) {
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
void draw_matrix(int *img, int x, int y) {
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

