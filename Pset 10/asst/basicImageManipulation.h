/* --------------------------------------------------------------------------
 * File:    basicImageManipulation.h
 * Created: 2015-09-23
 * --------------------------------------------------------------------------
 * 
 * 
 * 
 * ------------------------------------------------------------------------*/


#ifndef __basicImageManipulation__h
#define __basicImageManipulation__h

#include "Image.h"
#include <iostream>
#include <math.h>

using namespace std;

// --------- HANDOUT PS01 ------------------------------
Image brightness(const Image &im, float factor);
Image contrast(const Image &im, float factor, float midpoint = 0.5);

static const float weight_init[3] = {0.299, 0.587, 0.114};
Image color2gray(const Image &im, const std::vector<float> &weights = std::vector<float>(weight_init, weight_init+3));

std::vector<Image> lumiChromi(const Image &im);
Image lumiChromi2rgb(const vector<Image> & lc);
Image brightnessContrastLumi(const Image &im,
        float brightF, float contrastF, float midpoint = 0.3);
Image rgb2yuv(const Image &im);
Image yuv2rgb(const Image &im);
Image saturate(const Image &im, float k);
std::vector<Image> spanish(const Image &im);
Image grayworld(const Image & in);
// ------------------------------------------------------

// --------- HANDOUT PS05 ------------------------------
Image scaleNN(const Image &im, float factor);
Image scaleLin(const Image &im, float factor);
float interpolateLin(const Image &im, float x, float y, int z, bool clamp=false);
Image rotate(const Image &im, float theta); 
// ------------------------------------------------------

#endif
