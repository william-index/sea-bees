/**
 * Concerts rgb value to uint16_t color format
 * @param {int} red 0-255 color range
 * @param {int} green 0-255 color range
 * @param {int} blue 0-255 color range
 */
uint16_t rgb(int red, int green, int blue) {
  return (((31*(red+4))/255)<<11) | 
         (((63*(green+2))/255)<<5) | 
         ((31*(blue+4))/255);
}

uint16_t COLOR_BG = rgb(198,248,250);
uint16_t COLOR_MIDGREEN = rgb(29,214,160);
uint16_t COLOR_LIGHTGREEN = rgb(162,252,226);
uint16_t COLOR_DARKGREEN = rgb(24,162,105);
uint16_t COLOR_MIDPURPLE = rgb(208,139,231);
uint16_t COLOR_DARKPURPLE = rgb(190,110,216);
uint16_t COLOR_LIGHTPURPLE = rgb(241,197,255);
uint16_t COLOR_DARKBLUE = rgb(52,88,223);
uint16_t COLOR_BLUE = rgb(133,158,252);

uint16_t colors[] = {
  COLOR_BG,
  COLOR_MIDGREEN,
  COLOR_LIGHTGREEN,
  COLOR_DARKGREEN,
  COLOR_MIDPURPLE,
  COLOR_DARKPURPLE,
  COLOR_LIGHTPURPLE,
  COLOR_DARKBLUE,
  COLOR_BLUE
};

