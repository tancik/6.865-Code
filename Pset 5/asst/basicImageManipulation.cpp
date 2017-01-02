/* --------------------------------------------------------------------------
 * File:    basicImageManipulation.cpp
 * Created: 2015-09-23
 * --------------------------------------------------------------------------
 *
 *
 *
 * ------------------------------------------------------------------------*/


#include "basicImageManipulation.h"
using namespace std;


// --------- HANDOUT PS03 ------------------------------
// -----------------------------------------------------
//
Image scaleNN(const Image &im, float factor){
    // --------- HANDOUT  PS03 ------------------------------
    // create a new image that is factor times bigger than the input by using
    // nearest neighbor interpolation.
    int width = floor(factor * im.width());
    int height = floor(factor * im.height());
    Image newImage(width, height, im.channels());
    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        for (int c = 0; c < im.channels(); c++) {
          newImage(x,y,c) = im(x/factor, y/factor, c);
        }
      }
    }
    return newImage;
}

float interpolateLin(const Image &im, float x, float y, int z, bool clamp){
    // --------- HANDOUT  PS03 ------------------------------
    // bilinear interpolation samples the value of a non-integral
    // position (x,y) from its four "on-grid" neighboring pixels.
    //  |           |
    // -1-----------2-
    //  |           |  *: my coordinates (x,y) are not integral
    //  |  *        |     since I am not on the pixel grid :(
    //  |           |  1: top-left
    //  |           |  2: top-right
    //  |           |  3: bottom-right
    // -4-----------3- 4: bottom-left, what are our coordinates?
    //  |           |    We are willing to share some color
    //                   information with * ! Of course, the pixel
    //                   closest to * should influence it more.
    int x1 = floor(x);
    int x2 = ceil(x);
    int y1 = floor(y);
    int y2 = ceil(y);
    float fy1 = 0;
    float fy2 = 0;
    if (x1 != x2) {
      fy1 = (x2 - x)/(x2 - x1) * im.smartAccessor(x1, y1, z, clamp) + (x - x1)/(x2 - x1) * im.smartAccessor(x2, y1, z, clamp);
      fy2 = (x2 - x)/(x2 - x1) * im.smartAccessor(x1, y2, z, clamp) + (x - x1)/(x2 - x1) * im.smartAccessor(x2, y2, z, clamp);
    } else {
      fy1 = im.smartAccessor(x1, y1, z, clamp);
      fy2 = im.smartAccessor(x1, y2, z, clamp);
    }
    if (y1 == y2) {
      return fy1;
    }
    return (y2 - y)/(y2 - y1) * fy1 + (y - y1)/(y2 - y1) * fy2;
}

Image scaleLin(const Image &im, float factor){
    // --------- HANDOUT  PS03 ------------------------------
    // create a new image that is factor times bigger than the input by using
    // bilinear interpolation
    int width = floor(factor * im.width());
    int height = floor(factor * im.height());
    Image newImage(width, height, im.channels());
    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        for (int c = 0; c < im.channels(); c++) {
          newImage(x,y,c) = interpolateLin(im, x/factor, y/factor, c, true);
        }
      }
    }
    return newImage;
}


Image rotate(const Image &im, float theta) {
    // --------- HANDOUT  PS03 ------------------------------
    // (6.865 required, 6.815 extra credit)
    // rotate an image around its center by theta

  // center around which to rotate
    float centerX = (im.width()-1.0)/2.0;
    float centerY = (im.height()-1.0)/2.0;

    Image newImage(im.width(), im.height(), im.channels());

    for (int x = 0; x < im.width(); x++) {
      for (int y = 0; y < im.height(); y++) {
        float offX = x - centerX;
        float offY = y - centerY;
        float r = sqrt(pow(offX,2) + pow(offY,2));
        float targetTheta = 0;
        if (offX != 0) {
          // std::cout << "x: " << x << std::endl;
          // std::cout << "y: " << y << std::endl;
          targetTheta = atan(offY/offX) + theta;
          // std::cout << 180 / M_PI * targetTheta << std::endl;
          // std::cout << "x?: " << r * cos(targetTheta) + centerX << std::endl;
          // std::cout << "y?: " << r * sin(targetTheta) + centerY << std::endl;
        }
        for (int c = 0; c < im.channels(); c++) {
          if (offX < 0) {
            newImage(x,y,c) = interpolateLin(im, -r * cos(targetTheta) + centerX, -r * sin(targetTheta) + centerY, c, false);
          } else {
            newImage(x,y,c) = interpolateLin(im, r * cos(targetTheta) + centerX, r * sin(targetTheta) + centerY, c, false);
          }
        }
      }
    }
    return newImage; // changeme
}

// -----------------------------------------------------
// --------- END --- PS03 ------------------------------





// --- NO need to edit below, this is the solution to PS01 ---





// --------- HANDOUT PS01 ------------------------------
// -----------------------------------------------------

// Change the brightness of the image
// const Image & means a reference to im will get passed to the function,
// but the compiler won't let you modify it within the function.
// So you will return a new image
Image brightness(const Image &im, float factor) {
    // // --------- HANDOUT  PS01 ------------------------------
	// // Image output(im.width(), im.height(), im.channels());
	// // Modify image brightness
	// // return output;
	// return Image(1,1,1); // Change this

    // --------- SOLUTION PS01 ------------------------------
    return im * factor;
}

Image contrast(const Image &im, float factor, float midpoint) {
    // // --------- HANDOUT  PS01 ------------------------------
    // // Image output(im.width(), im.height(), im.channels());
    // // Modify image contrast
    // // return output;
	// return Image(1,1,1); //Change this

    // --------- SOLUTION PS01 ------------------------------
    return (im - midpoint)*factor+midpoint;
}

Image color2gray(const Image &im, const std::vector<float> &weights) {
    // // --------- HANDOUT  PS01 ------------------------------
    // // Image output(im.width(), im.height(), 1);
    // // Convert to grayscale
	// return Image(1,1,1); //Change this

    // --------- SOLUTION PS01 ------------------------------
    Image output(im.width(), im.height(), 1);
    for (int i = 0 ; i < im.width(); i++ ) {
        for (int j = 0 ; j < im.height(); j++ ) {
            output(i,j,0) = im(i,j,0) * weights[0] + im(i,j,1) * weights[1] + im(i,j,2) *weights[2];
        }
    }
    return output;
}

// For this function, we want two outputs, a single channel luminance image
// and a three channel chrominance image. Return them in a vector with luminance first
std::vector<Image> lumiChromi(const Image &im) {
    // // --------- HANDOUT  PS01 ------------------------------
    // // Create the luminance image
    // // Create the chrominance image
    // // Create the output vector as (luminance, chrominance)
	// return std::vector<Image>(); //Change this

    // --------- SOLUTION PS01 ------------------------------

    // Create the luminance
    Image im_luminance = color2gray(im);

    // Create chrominance images
    // We copy the input as starting point for the chrominance
    Image im_chrominance = im;
    for (int c = 0 ; c < im.channels(); c++ )
    for (int y = 0 ; y < im.height(); y++)
    for (int x = 0 ; x < im.width(); x++)
    {
        im_chrominance(x,y,c) = im_chrominance(x,y,c) / im_luminance(x,y);
    }

    // Stack luminance and chrominance in the output vector, luminance first
    std::vector<Image> output;
    output.push_back(im_luminance);
    output.push_back(im_chrominance);
    return output;
}


// Modify brightness then contrast
Image brightnessContrastLumi(const Image &im, float brightF, float contrastF, float midpoint) {
    // // --------- HANDOUT  PS01 ------------------------------
    // // Modify brightness, then contrast of luminance image
    // return Image(1,1,1); // Change this

    // --------- SOLUTION PS01 ------------------------------
    // Separate luminance and chrominance
    std::vector<Image> lumi_chromi = lumiChromi(im);
    Image im_luminance             = lumi_chromi[0];
    Image im_chrominance           = lumi_chromi[1];

    // Process the luminance channel
    im_luminance = brightness(im_luminance, brightF);
    im_luminance = contrast(im_luminance, contrastF, midpoint);

    // Multiply the chrominance with the new luminance to get the final image
    for (int i = 0 ; i < im.width(); i++ ){
        for (int j = 0 ; j < im.height(); j++) {
            for (int c = 0; c < im.channels(); c++) {
                im_chrominance(i,j,c) = im_chrominance(i,j,c) * im_luminance(i,j);
            }
        }
    }
    // At this point, im_chrominance olds the complete processed image
    return im_chrominance;
}


Image rgb2yuv(const Image &im) {
    // // --------- HANDOUT  PS01 ------------------------------
    // // Create output image of appropriate size
    // // Change colorspace
    // return Image(1,1,1); // Change this

    // --------- SOLUTION PS01 ------------------------------
    Image output(im.width(), im.height(), im.channels());
    for (int j = 0 ; j < im.height(); j++)
    for (int i = 0 ; i < im.width(); i++)
    {
        output(i,j,0) =   0.299 * im(i,j,0) + 0.587 * im(i,j,1) + 0.114 * im(i,j,2);
        output(i,j,1) = - 0.147 * im(i,j,0) - 0.289 * im(i,j,1) + 0.436 * im(i,j,2);
        output(i,j,2) =   0.615 * im(i,j,0) - 0.515 * im(i,j,1) - 0.100 * im(i,j,2);
    }
    return output;
}


Image yuv2rgb(const Image &im) {
    // // --------- HANDOUT  PS01 ------------------------------
    // // Create output image of appropriate size
    // // Change colorspace
    // return Image(1,1,1); // Change this

    // --------- SOLUTION PS01 ------------------------------
    Image output(im.width(), im.height(), im.channels());
    for (int j = 0 ; j < im.height(); j++)
    for (int i = 0; i < im.width(); i++)
    {
        output(i,j,0) =  im(i,j,0) + 0     * im(i,j,1) + 1.14  * im(i,j,2);
        output(i,j,1) =  im(i,j,0) - 0.395 * im(i,j,1) - 0.581 * im(i,j,2);
        output(i,j,2) =  im(i,j,0) + 2.032 * im(i,j,1) + 0     * im(i,j,2);
    }
    return output;
}


Image saturate(const Image &im, float factor) {
    // // --------- HANDOUT  PS01 ------------------------------
    // // Create output image of appropriate size
    // // Saturate image
    // // return output;
    // return Image(1,1,1); // Change this

    // --------- SOLUTION PS01 ------------------------------
    Image output = rgb2yuv(im); // Change colorspace
    for (int i = 0 ; i < im.width(); i++) {
        for (int j = 0 ; j < im.height(); j++) {
            output(i,j,1) = output(i,j,1) * factor;
            output(i,j,2) = output(i,j,2) * factor;
        }
    }
    output = yuv2rgb(output); // Back to RGB
    return output;
}


// Return two images in a C++ vector
std::vector<Image> spanish(const Image &im) {
    // // --------- HANDOUT  PS01 ------------------------------
    // // Remember to create the output images and the output vector
    // // Push the images onto the vector
    // // Do all the required processing
    // // Return the vector, color image first
	// return std::vector<Image>(); //Change this

    // --------- SOLUTION PS01 ------------------------------
    // Extract the luminance
    Image output_L = color2gray(im);

    // Convert to YUV for manipulation
    Image output_C = rgb2yuv(im);

    for (int j = 0; j < im.height(); j++)
    for (int i = 0; i < im.width(); i++)
    {
        output_C(i,j,0) = 0.5; // constant luminance
        output_C(i,j,1) = -output_C(i,j,1); // opposite chrominance
        output_C(i,j,2) = -output_C(i,j,2); // opposite chrominance
    }
    // Convert back to RGB
    output_C = yuv2rgb(output_C);

    // Location of the black dot
    int bdot_x = floor(im.width()/2);
    int bdot_y = floor(im.height()/2);

    // Add the black dot to Luminance, and Chrominance images
    output_L(bdot_x, bdot_y,0) = 0.0f;
    output_C(bdot_x, bdot_y,0) = 0.0f; // black is 0
    output_C(bdot_x, bdot_y,1) = 0.0f;
    output_C(bdot_x, bdot_y,2) = 0.0f;

    // Pack the images in a vector, chrominance first
    std::vector<Image> output;
    output.push_back(output_C);
    output.push_back(output_L);
    return output;
}


// White balances an image using the gray world assumption
Image grayworld(const Image & im) {
    // // --------- HANDOUT  PS01 ------------------------------
    // Implement automatic white balance by multiplying each channel
    // of the input by a factor such that the three channel of the output image
    // have the same mean value. The mean value of the green channel
    // is taken as reference.
    // return Image(1,1,1); // Change this

    // --------- SOLUTION PS01 ------------------------------
    // Compute the mean per channel
    float mean_r = 0, mean_g = 0, mean_b = 0;
    float N = im.width()*im.height();
    for (int j = 0 ; j < im.height(); j++)
    for (int i = 0 ; i < im.width(); i++)
    {
        mean_r += im(i,j,0);
        mean_g += im(i,j,1);
        mean_b += im(i,j,2);
    }
    mean_r /= N;
    mean_g /= N;
    mean_b /= N;

    Image output = im;
    for (int j = 0 ; j < im.height();j ++)
    for (int i = 0 ; i < im.width(); i++)
    {
        output(i,j,0) = output(i,j,0)/mean_r*mean_g;
        // dont process output(i,j,1), since the mean of
        // the green channel is already at the right value
        output(i,j,2) = output(i,j,2)/mean_b*mean_g;
    }
    return output;
}

// -----------------------------------------------------
// --------- END --- PS01 ------------------------------
