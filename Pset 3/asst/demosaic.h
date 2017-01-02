/* --------------------------------------------------------------------------
 * File:    demosaic.h
 * Created: 2015-10-01
 * --------------------------------------------------------------------------
 * 
 * 
 * 
 * ------------------------------------------------------------------------*/


#ifndef __demosaic__h
#define __demosaic__h

#include "Image.h"
#include "basicImageManipulation.h"
#include <iostream>
#include <math.h>

using namespace std;


Image basicGreen(const Image &raw, int offset=0);
Image basicRorB(const Image &raw, int offsetX, int offsetY);
Image basicDemosaic(const Image &raw, int offsetGreen=0, int offsetRedX=1, int offsetRedY=1, int offsetBlueX=0, int offsetBlueY=0);
Image edgeBasedGreen(const Image &raw, int offset=0);
Image edgeBasedGreenDemosaic(const Image &raw, int offsetGreen=0, int offsetRedX=1, int offsetRedY=1, int offsetBlueX=0, int offsetBlueY=0);
Image greenBasedRorB(const Image &raw, Image &green, int offsetX, int offsetY);
Image improvedDemosaic(const Image &raw, int offsetGreen=0, int offsetRedX=1, int offsetRedY=1, int offsetBlueX=0, int offsetBlueY=0);

 
#endif
