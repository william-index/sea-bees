#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

typedef struct BeeData BeeData;

struct BeeData {
  char file[4];
  int evolutions[2];
  int carryCapacity;
  int evolveThreshold;
};

BeeData *beeDict[4];
BeeData baba, babb, prwn, puff, aaaa, b, c, d, e, f, g, h;

class Bee {
  public: 
    String beeName;
    char file[4];
    char addRes[1]; // protexts file name memory space? Why is this even needed
    int heldPollen, maxPollen, index, collectedPollen;
  public:
    Bee (int);
    char get_index() {return index;}
    void evolve() {
      index = beeDict[index]->evolutions[random(2)];
      heldPollen = 0;  
      collectedPollen = 0;
      maxPollen = 0;
    
      if (index > -1) {
        strcpy(file, beeDict[index]->file);
        maxPollen = beeDict[index]->carryCapacity;
      }
    }
    void set_bee_name(String name) {
      beeName = name;
    }
};

Bee::Bee (int bIndex) {
  index = bIndex;
  heldPollen = 0;  
  collectedPollen = 0;
  maxPollen = 0;

  if (bIndex > -1) {
    strcpy(file, beeDict[bIndex]->file);
    maxPollen = beeDict[bIndex]->carryCapacity;
  }
}

