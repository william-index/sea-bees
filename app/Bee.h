#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

typedef struct BeeData BeeData;

struct BeeData {
  char file[4];
  char overflowSafety; // super hacky, I dont know, this saves memory from overflowing haha
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
    /**
     * Evolves a bee randomly via one of its possible evolution paths
     */
    void evolve(int bIndex) {
      if (bIndex < -1) {bIndex = -1;}
      set_bee(bIndex);
    }

    /**
     * Starts a bee's life anew for the bee of the given index type
     */
    void startBee(int beeType) {
      set_bee(beeType);
    }
    void set_bee(int beeIndex) {
      index = beeIndex;
      heldPollen = 0;  
      collectedPollen = 0;
      maxPollen = 1;
    
      if (index > -1) {
        strcpy(file, beeDict[index]->file);
        maxPollen = beeDict[index]->carryCapacity;
      } else {
        strcpy(file, "eelb");
      }
    }
};

Bee::Bee (int bIndex) {
  set_bee(bIndex);
}

