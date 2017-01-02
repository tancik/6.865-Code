/* -----------------------------------------------------------------
 * File:    filtering.cpp
 * Created: 2015-09-22
 * -----------------------------------------------------------------
 *
 * Image convolution and filtering
 *
 * ---------------------------------------------------------------*/


#include "filtering.h"
#include <cmath>
#include <cassert>

using namespace std;

Image boxBlur(const Image &im, int k, bool clamp) {
    // --------- HANDOUT  PS02 ------------------------------
    // convolve an image with a box filter of size k by k
    Image newImage = Image(im.width(), im.height(), im.channels());
    for (int x = 0; x < im.width(); x++) {
      for (int y = 0; y < im.height(); y++) {
        for (int c = 0; c < im.channels(); c++) {
          float val = 0.0;
          for (int i = -k / 2; i <= k / 2; i++) {
            for (int j = -k / 2; j <= k / 2; j++) {
              val += im.smartAccessor(x + i, y + j, c, clamp);
            }
          }
          newImage(x,y,c) = val / (k * k);
        }
      }
    }
    return newImage; // change this
}

Image Filter::convolve(const Image &im, bool clamp){
    // --------- HANDOUT  PS02 ------------------------------
    // Write a convolution function for the filter class
    Image newImage = Image(im.width(), im.height(), im.channels());
    for (int x = 0; x < im.width(); x++) {
      for (int y = 0; y < im.height(); y++) {
        for (int c = 0; c < im.channels(); c++) {
          float val = 0.0;
          for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
              val += im.smartAccessor(x + i - width/2, y + j - height/2, c, clamp) * operator()(width - i -1, height - j -1);
            }
          }
          newImage(x,y,c) = val;
        }
      }
    }
    return newImage;
}



Image boxBlur_filterClass(const Image &im, int k, bool clamp) {
    // --------- HANDOUT  PS02 ------------------------------
    // Reimplement the box filter using the filter class.
    // check that your results match those in the previous function "boxBlur"
    vector<float> kernel; // C++11 syntax
    kernel.assign (k * k,1.0/(k * k));
    Filter testFilter(kernel, k, k);
    Image newImage = testFilter.convolve(im, clamp);
    return newImage;
}


Image gradientMagnitude(const Image &im, bool clamp){
    // --------- HANDOUT  PS02 ------------------------------
    // Uses a Sobel kernel to compute the horizontal and vertical
    // components of the gradient of an image and returns the gradient magnitude.
    Image newImage = Image(im.width(), im.height(), im.channels());
    vector<float> xkernel{-1,0,1,
                          -2,0,2,
                          -1,0,1};
    vector<float> ykernel{-1,-2,-1,
                          0,0,0,
                          1,2,1};
    Filter xFilter(xkernel, 3, 3);
    Filter yFilter(ykernel, 3, 3);
    Image xSobel = xFilter.convolve(im);
    Image ySobel = yFilter.convolve(im);
    for (int x = 0; x < im.width(); x++) {
      for (int y = 0; y < im.height(); y++) {
        for (int c = 0; c < im.channels(); c++) {
          newImage(x,y,c) = sqrt(pow(xSobel(x,y,c),2) + pow(ySobel(x,y,c), 2));
        }
      }
    }
    return newImage;
}

vector<float> gauss1DFilterValues(float sigma, float truncate){
    // --------- HANDOUT  PS02 ------------------------------
    // Create a vector containing the normalized values in a 1D Gaussian filter
    // Truncate the gaussian at truncate*sigma.
    vector<float> values(1 + 2 * ceil(sigma * truncate));
    float normCount = 0;
    for (int i = 0; i <= ceil(sigma * truncate); i++) {
      float val = exp(-pow(i,2) / (2 * pow(sigma, 2)));
      if (i != 0) {
        values.at(sigma * truncate + i) = val;
        values.at(sigma * truncate - i) = val;
        normCount += 2 * val;
      } else {
        values.at(sigma * truncate) = val;
        normCount += val;
      }
    }
    for (int i = 0; i < (1+2*ceil(sigma*truncate)); i++) {
      values.at(i) = values.at(i) / normCount;
    }
    return values;
}

Image gaussianBlur_horizontal(const Image &im, float sigma, float truncate, bool clamp){
    // --------- HANDOUT  PS02 ------------------------------
    // Gaussian blur across the rows of an image
    vector<float> kernel = gauss1DFilterValues(sigma, truncate);
    Filter testFilter(kernel, 1+2*ceil(sigma * truncate), 1);
    Image newImage = testFilter.convolve(im, clamp);
    return newImage;
}

vector<float> gauss2DFilterValues(float sigma, float truncate){
    // --------- HANDOUT  PS02 ------------------------------
    // create a vector containing the normalized values in a 2D Gaussian
    // filter. Truncate the gaussian at truncate*sigma.
    int kernelWidth = 1+2*ceil(sigma*truncate);
    vector<float> one = gauss1DFilterValues(sigma, truncate);
    vector<float> two(pow(kernelWidth,2));
    float norm = 0;
    for (int i = 0; i < kernelWidth; i++) {
      for (int j = 0; j < kernelWidth; j++) {
        two.at(i + j * kernelWidth) = one.at(i) * one.at(j);
        norm += one.at(i) * one.at(j);
      }
    }
    return two;
}


Image gaussianBlur_2D(const Image &im, float sigma, float truncate, bool clamp){
    // --------- HANDOUT  PS02 ------------------------------
    //  Blur an image with a full  full 2D rotationally symmetric Gaussian kernel
    vector<float> kernel = gauss2DFilterValues(sigma, truncate);
    Filter testFilter(kernel, 1+2*ceil(sigma * truncate), 1+2*ceil(sigma * truncate));
    Image newImage = testFilter.convolve(im, clamp);
    return newImage;
}

Image gaussianBlur_separable(const Image &im, float sigma, float truncate, bool clamp){
    // --------- HANDOUT  PS02 ------------------------------
    // Use principles of seperabiltity to blur an image using 2 1D Gaussian Filters
    vector<float> kernel = gauss1DFilterValues(sigma, truncate);
    Filter xFilter(kernel, 1+2*ceil(sigma * truncate), 1);
    Filter yFilter(kernel, 1, 1+2*ceil(sigma * truncate));
    Image newImage = yFilter.convolve(xFilter.convolve(im, clamp), clamp);
    return newImage;
}


Image unsharpMask(const Image &im, float sigma, float truncate, float strength, bool clamp){
    // --------- HANDOUT  PS02 ------------------------------
    // sharpen an image
    Image blured = gaussianBlur_separable(im, sigma, truncate, clamp);
    Image highPass = im - blured;
    Image newImage = im + strength * highPass;
    return newImage;
}

Image bilaYUV(const Image &im, float sigmaRange, float sigmaY, float sigmaUV, float truncateDomain, bool clamp){
    // --------- HANDOUT  PS02 ------------------------------
    // 6.865 only
    // Bilaterial Filter an image seperatly for
    // the Y and UV components of an image
    Image newImage = Image(im.width(), im.height(), im.channels());
    Image yuv = rgb2yuv(im);
    vector<float> fy = gauss2DFilterValues(sigmaY, truncateDomain);
    vector<float> fuv = gauss2DFilterValues(sigmaUV, truncateDomain);
    int y_kernelLen = sqrt(fy.size());
    int y_mid = y_kernelLen/2;
    int uv_kernelLen = sqrt(fuv.size());
    int uv_mid = uv_kernelLen/2;
    for (int x = 0; x < yuv.width(); x++) {
      for (int y = 0; y < yuv.height(); y++) {
        for (int c = 0; c < yuv.channels(); c++) {
          float sum = 0;
          float sumWeights = 0;
          if (c == 0) {
            for (int j = 0; j < y_kernelLen; j++) {
              for (int i = 0; i < y_kernelLen; i++) {
                float colorsSqr = 0;
                for (int c2 = 0; c2 < yuv.channels(); c2++) {
                  colorsSqr += pow(yuv.smartAccessor(x,y,c2,clamp) - yuv.smartAccessor(x+i-y_mid,y+j-y_mid,c2,clamp), 2);
                }
                colorsSqr = sqrt(colorsSqr);
                float g = exp(-pow(colorsSqr,2) / (2 * pow(sigmaRange, 2)));
                float inter = 0;
                inter = fy.at(i + j * y_kernelLen) * g;
                sum += inter * yuv.smartAccessor(x + i - y_mid, y + j - y_mid, c, clamp);
                sumWeights += inter;
              }
            }
          } else {
            for (int j = 0; j < uv_kernelLen; j++) {
              for (int i = 0; i < uv_kernelLen; i++) {
                float colorsSqr = 0;
                for (int c2 = 0; c2 < yuv.channels(); c2++) {
                  colorsSqr += pow(yuv.smartAccessor(x,y,c2,clamp) - yuv.smartAccessor(x+i-uv_mid,y+j-uv_mid,c2,clamp), 2);
                }
                colorsSqr = sqrt(colorsSqr);
                float g = exp(-pow(colorsSqr,2) / (2 * pow(sigmaRange, 2)));
                float inter = 0;
                inter = fuv.at(i + j * uv_kernelLen) * g;
                sum += inter * yuv.smartAccessor(x + i - uv_mid, y + j - uv_mid, c, clamp);
                sumWeights += inter;
              }
            }
          }
          newImage(x,y,c) = sum / sumWeights;
        }
      }
    }
    return yuv2rgb(newImage);
}


Image bilateral(const Image &im, float sigmaRange, float sigmaDomain, float truncateDomain, bool clamp){
    // --------- HANDOUT  PS02 ------------------------------
    // Denoise an image using the bilateral filter
    Image newImage = Image(im.width(), im.height(), im.channels());
    vector<float> f = gauss2DFilterValues(sigmaDomain, truncateDomain);
    int kernelLen = sqrt(f.size());
    int mid = kernelLen/2;
    for (int x = 0; x < im.width(); x++) {
      for (int y = 0; y < im.height(); y++) {
        for (int c = 0; c < im.channels(); c++) {
          float sum = 0;
          float sumWeights = 0;
          for (int j = 0; j < kernelLen; j++) {
            for (int i = 0; i < kernelLen; i++) {
              float colorsSqr = 0;
              for (int c2 = 0; c2 < im.channels(); c2++) {
                colorsSqr += pow(im.smartAccessor(x,y,c2,clamp) - im.smartAccessor(x+i-mid,y+j-mid,c2,clamp), 2);
              }
              colorsSqr = sqrt(colorsSqr);
              float g = exp(-pow(colorsSqr,2) / (2 * pow(sigmaRange, 2)));
              float inter = f.at(i + j * kernelLen) * g;
              sum += inter * im.smartAccessor(x + i - mid, y + j - mid, c, clamp);
              sumWeights += inter;
            }
          }
          newImage(x,y,c) = sum / sumWeights;
        }
      }
    }
    return newImage;
}

Image medianFilter(const Image &im, int k, bool clamp) {
  Image newImage = Image(im.width(), im.height(), im.channels());
  for (int x = 0; x < im.width(); x++) {
    for (int y = 0; y < im.height(); y++) {
      for (int c = 0; c < im.channels(); c++) {
        vector<float> values;
        for (int i = 0; i < k; i++) {
          for (int j = 0; j < k; j++) {
            values.push_back(im.smartAccessor(x + i - k/2, y + j - k/2, c, clamp));
          }
        }
        std::sort(values.begin(), values.end());
        newImage(x,y,c) = values.at((k*k)/2);
      }
    }
  }
  return newImage;
}



/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// Create an image of 0's with a value of 1 in the middle. This function
// can be used to test that you have properly set the kernel values in your
// Filter object. Make sure to set k to be larger than the size of your kernel
Image impulseImg(int k){
    // initlize a kxkx1 image of all 0's
    Image impulse(k, k, 1);

    // set the center pixel to have intensity 1
    int center = floor(k/2);
    impulse(center,center,0) = 1.0f;

    return impulse;
}


// ------------- FILTER CLASS -----------------------
Filter::Filter(const vector<float> &fData, int fWidth, int fHeight)
    : kernel(fData), width(fWidth), height(fHeight)
{
        assert(fWidth*fHeight == fData.size());
}


Filter::Filter(int fWidth, int fHeight)
    : kernel(std::vector<float>(fWidth*fHeight,0)), width(fWidth), height(fHeight) {}


Filter::~Filter() {}


const float & Filter::operator()(int x, int y) const {
    if (x < 0 || x >= width)
        throw OutOfBoundsException();
    if ( y < 0 || y >= height)
        throw OutOfBoundsException();

    return kernel[x + y*width];
}


float & Filter::operator()(int x, int y) {
    if (x < 0 || x >= width)
        throw OutOfBoundsException();
    if ( y < 0 || y >= height)
        throw OutOfBoundsException();

    return kernel[x +y*width];
}
// --------- END FILTER CLASS -----------------------
