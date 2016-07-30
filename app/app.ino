// Sea Bees Virtual pet
// Pro Trinket 3V, 12MHz
// Adafruit 1.8" Color TFT Display (128px x 128px)
//-----------------------------------------------------------

// Includes
//-----------------------------------------------------------
#include "color.h"
#include "graphics.h"
#include "Bee.h"

// Hardware setup I dont understand
//-----------------------------------------------------------
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <EEPROM.h>
#include <SD.h>


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    9  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     8
#define SD_CS    4  // Chip select line for SD card

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
#define TFT_MISO 12
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);



// Global Variables
//-----------------------------------------------------------
#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )

//globals
int test = 0;
int pollenCounter = 0;
int playerPollen = 0;
int showGather = 0;
const int pollenAddRate = 4;

//buttons
const int buttonPin1 = 5;
const int buttonPin2 = 6;
const int buttonPin3 = A0;

const int screenBrightness = 9;

bool canPress[3] = {true,true,true};
bool pressedBtns[3] = {false,false,false};
bool btnstates[3];

//animation
int frame = 0;
int prevframe = 0;
const long FRAME_RATE = 500; 
const int MAX_FRAMES = 4;
unsigned long previousMillis = 0; 

int bmpstatus = 0;
boolean firstbmp = false;
int bmp1[3];

//application states
int state = 0;
int onBee = 0;


Bee *bee[3];

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

  pinMode(screenBrightness, OUTPUT);
  analogWrite(screenBrightness, 180); // % 255

  if (!SD.begin(SD_CS)) {
    bmpstatus=503;
    return;
  }

  setBeeData();
    //updateFrames();
  bee[0] = new Bee(0);
  bee[1] = new Bee(-1);
  bee[2] = new Bee(-1);
  //  uint8_t read
//  test = EEPROM.read(0);
}

/**
 * Sets up data for bee types
 */
void setBeeData() {
  // 0 - babya
  baba.carryCapacity = 1;
  baba.evolutions[0] = 2;
  baba.evolutions[1] = 2;
  baba.evolveThreshold = 6;
  strcpy(baba.file, "grub");
  beeDict[0] = &baba;

  // 1 - babyb
  babb.carryCapacity = 2;
  babb.evolutions[0] = 3;
  babb.evolutions[1] = 5;
  babb.evolveThreshold = 10;
  strcpy(babb.file, "babb");
  beeDict[1] = &babb;

  // 2 - puff
  puff.carryCapacity = 5;
  puff.evolutions[0] = 6;
  puff.evolutions[1] = 7;
  puff.evolveThreshold = 100;
  strcpy(puff.file, "puff");
  beeDict[2] = &puff;
}


/**
 * Main application loop
 */
void loop() {
  updateFrames();
  btnPresses();
  updateState();  
  drawCurrentState();
}


/**
 * Draws various dev states to the screen, 
 *  helpful for debugging
 */
void drawDevState() {
  
//    bmpDraw("sambee.bmp", 0, 0);
  
  
//  //  Button test output
  tft.setCursor(0,0);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_BLUE,COLOR_BG);
  tft.print("pollen:");
  tft.print(playerPollen);

  tft.setCursor(0,64);

  tft.print("bee:0 index:");
  tft.print(int((*bee[0]).get_index()));
  tft.println("");
  tft.print("bee:0 collected:");
  tft.print(bee[0]->collectedPollen);
  tft.print("/");
  tft.print( beeDict[int((*bee[0]).get_index())]->evolveThreshold );
}


/**
 * Routes to routines for drawing the current state
 */
void drawCurrentState() {
  if (state == 0) {
    drawDevState();
  } else if (state == 1) {
    drawPlantState();
  } else if (state == 2) {
    drawBeeState();
  } else if (state == 3) {
    drawAddBeeState();
  }
}

/** 
 * Screen to add a new bee
 */
void drawAddBeeState() {
  drawArrows();
  bmpDraw("addbee.bmp", 6, 6);
}

/** 
 * Screen to display plant
 */
void drawPlantState() {
  tft.setCursor(0,0);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_BLUE,COLOR_BG);
  tft.print("plant");
}

/** 
 * Progresses State based on button presses and previous states 
 * 0 - start screen
  * 1 - plant view
  * 2 - bee view
  * 3 - bee selection
  * 4 - admin
 */
void updateState() {  
  int prevState = state;

  if (pressedBtns[1]) {
    state++;
    state = state % 4;
  }

  // cycle Bees on bee state
  if (state == 2 || state == 3) {
    beeManagement();
  }

  if (state != prevState) {
    tft.fillScreen(COLOR_BG);
  }
}

/**
 * Handles the management of the bee screens
 */
void beeManagement() {
  if (pressedBtns[0]) { onBee = onBee - 1; }
  if (pressedBtns[2]) { onBee = onBee + 1; } 

  // modulo bee cycle and switch to add state if invalid bee
  onBee = onBee % 3;
  if (bee[abs(onBee)]->index == -1) {
    state = 3;
  } else {
    state = 2;
    checkActiveBeeEvolution();
  }

  gatherPollen();
}

void checkActiveBeeEvolution() {
  if (bee[onBee]->collectedPollen >= beeDict[int((*bee[onBee]).get_index())]->evolveThreshold) {
    bee[onBee]->collectedPollen = 0;
    bee[onBee]->evolve();
  }
}

/**
 * Handles actions and checks for gathering pollen from bees
 */
void gatherPollen() {
  if (state == 2 && pressedBtns[1] && bee[abs(onBee)]->heldPollen > 0) {
    tft.fillRect(0, 4*8, 128, 4*16, COLOR_LIGHTPURPLE);  
    
    showGather = bee[abs(onBee)]->heldPollen;
    playerPollen += showGather;
    bee[abs(onBee)]->collectedPollen += showGather;
    bee[abs(onBee)]->heldPollen = 0;
  } else if (showGather > 0 && pressedBtns[1]) {
    tft.fillRect(0, 4*8, 128, 4*16, COLOR_LIGHTGREEN);
    
    showGather = 0;
  }
}

/**
 * Handles Drawing the Bee State
 */
void drawBeeState() {
  char fileName[10];
  
  if (showGather > 0) {
    tft.setCursor(5*8,6*8);
    tft.setTextSize(4);
    tft.setTextColor(COLOR_DARKBLUE,COLOR_LIGHTPURPLE);
    tft.print("+"); tft.print(showGather);
  } else { 
     drawArrows();
 
    if (prevframe != frame) {
      if (frame < 2) {
        getBeeFile(0, '1').toCharArray(fileName, 10);
      } else {
        getBeeFile(0, '2').toCharArray(fileName, 10);
      }
  
      bmpDraw(fileName, 6, 6);
      prevframe = frame;
    }
    
    //  show bee pollen
    int xOffset = ((128 - ((bee[abs(onBee)]->maxPollen*12)-4))/2)/4;
    for (int i=0; i<bee[abs(onBee)]->maxPollen; i++) {
      if (bee[abs(onBee)]->heldPollen > i) {
        bmpDraw("polf.bmp", (i*3)+xOffset, 32-4); 
      } else {
        bmpDraw("pole.bmp", (i*3)+xOffset, 32-4);     
      }
    }
  }
}


/**
 * draws arrows to the screen
 */
void drawArrows() {
  draw_matrix(leftArrow,1,15);
  draw_matrix(rightArrow,28,15);

  for (int i=0; i<3; i++) {
    if (abs(onBee) == i) {
      tft.fillRect((i*12)+4, 4, 8, 8, COLOR_DARKGREEN);  
    } else {
      tft.fillRect((i*12)+4, 4, 8, 8, COLOR_MIDGREEN);  
    }
  }
}

/**
 * Returns File name for bee file
 */
String getBeeFile(char beeIndex, char frame) {
  return String(bee[beeIndex]->file) + frame + ".bmp";
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
      updatePollen();
    }
  }
}

/**
 * Updates the pollen for all active bees
 */
void updatePollen() {
  pollenCounter = (pollenCounter + 1) % pollenAddRate;

  if (pollenCounter == 0) {
    for (int i=0; i<3; i++) {
      if (bee[i]->index != -1
          && bee[i]->heldPollen < bee[i]->maxPollen) {
        bee[i]->heldPollen++;
      }
    }
  }

  return;
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



// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20


void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  char drawRow;

  x = x * 4;
  y = y * 4;
  if((x >= tft.width()) || (y >= tft.height())) return;

  bmpstatus=100;

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    bmpstatus=404;
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    bmpstatus=200;
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+(w*4)-1, y+(h*4)-1);

        for (row=0; row<h; row++) { // For each scanline...
          drawRow = row*4;
          
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
//          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
          pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
//          else     // Bitmap is stored top-to-bottom
//            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];

            if (r!=198 && g!=248 && b!=250) {
              tft.fillRect(col*4 + x, row*4 + y, 4, 4, tft.Color565(r,g,b));
            } else {
              tft.fillRect(col*4 + x, row*4 + y, 4, 4, COLOR_BG);
            }
            
          } // end pixel
        } // end scanline
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) bmpstatus=500;
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}



