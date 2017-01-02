/* --------------------------------------------------------------------------
 * File:    demosaic.cpp
 * Created: 2015-10-01
 * --------------------------------------------------------------------------
 *
 *
 *
 * ------------------------------------------------------------------------*/


#include "demosaic.h"
#include <cmath>

using namespace std;


Image basicGreen(const Image &raw, int offset){
    // --------- HANDOUT  PS03 ------------------------------
    // Takes as input a raw image and returns a single-channel
    // 2D image corresponding to the green channel using simple interpolation
    Image newImage(raw.width(), raw.height(), 1);
    for (int y = 0; y < (raw.height()); y++) {
      for (int x = 0; x < (raw.width()); x++) {
        if (x == 0 || y==0 || x == (raw.width() - 1) || y == (raw.height() - 1)) {
          newImage(x,y) = raw(x,y);
        } else {
          if ((x + y%2) % 2 == offset) {
            newImage(x,y) = raw(x,y);
          } else {
            float sum = raw(x-1,y) + raw(x+1,y) + raw(x,y-1) + raw(x,y+1);
            newImage(x,y) = sum / 4.0;
          }
        }
      }
    }
    return newImage;

}


Image basicRorB(const Image &raw, int offsetX, int offsetY){
    // --------- HANDOUT  PS03 ------------------------------
    //  Takes as input a raw image and returns a single-channel
    //  2D image corresponding to the red or blue channel using simple interpolation
    Image newImage(raw.width(), raw.height(), 1);
    for (int y = 0; y < (raw.height()); y++) {
      for (int x = 0; x < (raw.width()); x++) {
        if (x == 0 || y==0 || x == (raw.width() - 1) || y == (raw.height() - 1)) {
          newImage(x,y) = raw(x,y);
        } else {
          if (x%2 != offsetX && y%2 != offsetY) {
            float sum = raw(x-1,y-1) + raw(x+1,y-1) + raw(x-1,y+1) + raw(x+1,y+1);
            newImage(x,y) = sum / 4.0;
          } else if (x%2 == offsetX && y%2 != offsetY) {
            newImage(x,y) = (raw(x,y-1) + raw(x,y+1))/2;
          } else if (x%2 != offsetX && y%2 == offsetY) {
            newImage(x,y) = (raw(x-1,y) + raw(x+1,y))/2;
          } else {
            newImage(x,y) = raw(x,y);
          }
        }
      }
    }
    return newImage;
}


Image basicDemosaic(const Image &raw, int offsetGreen, int offsetRedX, int offsetRedY, int offsetBlueX, int offsetBlueY){
    // --------- HANDOUT  PS03 ------------------------------
    // takes as input a raw image and returns an rgb image
    // using simple interpolation to demosaic each of the channels
    Image newImage(raw.width(), raw.height(), 3);
    Image green = basicGreen(raw, offsetGreen);
    Image red = basicRorB(raw, offsetRedX, offsetRedY);
    Image blue = basicRorB(raw, offsetBlueX, offsetBlueY);
    for (int y = 0; y < green.height(); y++) {
      for (int x = 0; x < green.width(); x++) {
        newImage(x,y,0) = red(x,y);
        newImage(x,y,1) = green(x,y);
        newImage(x,y,2) = blue(x,y);
      }
    }
    return newImage;
}

Image edgeBasedGreen(const Image &raw, int offset){
    // --------- HANDOUT  PS03 ------------------------------
    // Takes a raw image and outputs a single-channel
    // image corresponding to the green channel taking into account edges
    Image newImage(raw.width(), raw.height(), 1);
    for (int y = 0; y < (raw.height()); y++) {
      for (int x = 0; x < (raw.width()); x++) {
        if (x == 0 || y==0 || x == (raw.width() - 1) || y == (raw.height() - 1)) {
          newImage(x,y) = raw(x,y);
        } else {
          if ((x + y%2) % 2 == offset) {
            newImage(x,y) = raw(x,y);
          } else {
            float sum1 = abs(raw(x-1,y) - raw(x+1,y));
            float sum2 = abs(raw(x,y-1) - raw(x,y+1));
            float result = 0;
            if (sum1 < sum2) {
              result = (raw(x-1,y) + raw(x+1,y)) / 2.0;
            } else {
              result = (raw(x,y-1) + raw(x,y+1)) / 2.0;
            }
            newImage(x,y) = result;
          }
        }
      }
    }
    return newImage;

}

Image edgeBasedGreenDemosaic(const Image &raw, int offsetGreen, int offsetRedX, int offsetRedY, int offsetBlueX, int offsetBlueY){
    // --------- HANDOUT  PS03 ------------------------------
    // Takes as input a raw image and returns an rgb image
    // using edge-based green demosaicing for the green channel and
    // simple interpolation to demosaic the red and blue channels
    Image newImage(raw.width(), raw.height(), 3);
    Image green = edgeBasedGreen(raw, offsetGreen);
    Image red = basicRorB(raw, offsetRedX, offsetRedY);
    Image blue = basicRorB(raw, offsetBlueX, offsetBlueY);
    for (int y = 0; y < green.height(); y++) {
      for (int x = 0; x < green.width(); x++) {
        newImage(x,y,0) = red(x,y);
        newImage(x,y,1) = green(x,y);
        newImage(x,y,2) = blue(x,y);
      }
    }
    return newImage;
}


Image greenBasedRorB(const Image &raw, Image &green, int offsetX, int offsetY){
    // --------- HANDOUT  PS03 ------------------------------
    // Takes as input a raw image and returns a single-channel
    // 2D image corresponding to the red or blue channel using green based interpolation
    Image newImage(raw.width(), raw.height(), 1);
    for (int y = 0; y < (raw.height()); y++) {
      for (int x = 0; x < (raw.width()); x++) {
        if (x == 0 || y==0 || x == (raw.width() - 1) || y == (raw.height() - 1)) {
          newImage(x,y) = raw(x,y);
        } else {
          if (x%2 != offsetX && y%2 != offsetY) {
            float sum = raw(x-1,y-1)-green(x-1,y-1) + raw(x+1,y-1)-green(x+1,y-1) + raw(x-1,y+1)-green(x-1,y+1) + raw(x+1,y+1)-green(x+1,y+1);
            newImage(x,y) = sum / 4.0 + green(x,y);
          } else if (x%2 == offsetX && y%2 != offsetY) {
            newImage(x,y) = (raw(x,y-1)-green(x,y-1) + raw(x,y+1)-green(x,y+1))/2 + green(x,y);
          } else if (x%2 != offsetX && y%2 == offsetY) {
            newImage(x,y) = (raw(x-1,y)-green(x-1,y) + raw(x+1,y)-green(x+1,y))/2 + green(x,y);
          } else {
            newImage(x,y) = raw(x,y);
          }
        }
      }
    }
    return newImage;
}

Image improvedDemosaic(const Image &raw, int offsetGreen, int offsetRedX, int offsetRedY, int offsetBlueX, int offsetBlueY){
    // --------- HANDOUT  PS03 ------------------------------
    // Takes as input a raw image and returns an rgb image
    // using edge-based green demosaicing for the green channel and
    // simple green based demosaicing of the red and blue channels
    Image newImage(raw.width(), raw.height(), 3);
    Image green = edgeBasedGreen(raw, offsetGreen);
    Image red = greenBasedRorB(raw, green, offsetRedX, offsetRedY);
    Image blue = greenBasedRorB(raw, green, offsetBlueX, offsetBlueY);
    for (int y = 0; y < green.height(); y++) {
      for (int x = 0; x < green.width(); x++) {
        newImage(x,y,0) = red(x,y);
        newImage(x,y,1) = green(x,y);
        newImage(x,y,2) = blue(x,y);
      }
    }
    return newImage;
}
