/* -----------------------------------------------------------------
 * File:    a1.cpp
 * Created: 2015-09-15
 * -----------------------------------------------------------------
 *
 * Assignment 01
 *
 * ---------------------------------------------------------------*/


#include "a1.h"
using namespace std;

// Change the brightness of the image
// const Image & means a reference to im will get passed to the function,
// but the compiler won't let you modify it within the function.
// So you will return a new image
Image brightness(const Image &im, float factor) {
    // --------- HANDOUT  PS01 ------------------------------
	// Image output(im.width(), im.height(), im.channels());
	// Modify image brightness
	// return output;
	Image myImage = Image(im.width(), im.height(), im.channels());
	for (int i = 0; i < im.number_of_elements(); i++) {
		myImage(i) = im(i) * factor;
	}
	return myImage;
}


Image contrast(const Image &im, float factor, float midpoint) {
    // --------- HANDOUT  PS01 ------------------------------
    // Image output(im.width(), im.height(), im.channels());
    // Modify image contrast
    // return output;
		Image myImage = Image(im.width(), im.height(), im.channels());
		for (int i = 0; i < im.number_of_elements(); i++) {
			float newValue = factor * (im(i) - midpoint) + midpoint;
			if (newValue < 0) {
				newValue = 0;
			} else if (newValue > 1.0) {
				newValue = 1.0;
			}
			myImage(i) = factor * (im(i) - midpoint) + midpoint;
		}
		return myImage;
}


Image color2gray(const Image &im, const std::vector<float> &weights) {
	Image myImage = Image(im.width(), im.height(), 1);
	for (int x = 0; x < im.width(); x++) {
		for (int y = 0; y < im.height(); y++) {
			float avg = 0;
			for (int c = 0; c < im.channels(); c++) {
				avg += im(x, y, c);
			}
			avg /= im.channels();
			myImage(x,y) = avg;
		}
	}
	return myImage;
}


// For this function, we want two outputs, a single channel luminance image
// and a three channel chrominance image. Return them in a vector with luminance first
std::vector<Image> lumiChromi(const Image &im) {
    // --------- HANDOUT  PS01 ------------------------------
    // Create the luminance image
    // Create the chrominance image
    // Create the output vector as (luminance, chrominance)
	Image greyImage = color2gray(im);
	Image lumImage = Image(im.width(), im.height(), im.channels());
	for (int x = 0; x < im.width(); x++) {
		for (int y = 0; y < im.height(); y++) {
			for (int c = 0; c < im.channels(); c++) {
				lumImage(x,y,c) = im(x,y,c) / greyImage(x,y);
			}
		}
	}
	std::vector<Image> images;
	images.push_back(lumImage);
	images.push_back(greyImage);
	return images;
}


// Modify brightness then contrast
Image brightnessContrastLumi(const Image &im, float brightF, float contrastF, float midpoint) {
    // --------- HANDOUT  PS01 ------------------------------
    // Modify brightness, then contrast of luminance image
		Image newImage = Image(im.width(), im.height(), im.channels());
		std::vector<Image> decomp = lumiChromi(im);
		Image greyImage = decomp[0];
		Image lumImage = decomp[1];
		Image newGrey = contrast(brightness(im, brightF), contrastF, midpoint);
		for (int x = 0; x < im.width(); x++) {
			for (int y = 0; y < im.height(); y++) {
				for (int c = 0; c < im.channels(); c++) {
					newImage(x,y,c) = im(x,y,c) * newGrey(x,y);
				}
			}
		}
		return newImage;
}


Image rgb2yuv(const Image &im) {
    // --------- HANDOUT  PS01 ------------------------------
    // Create output image of appropriate size
    // Change colorspace
		Image newImage = Image(im.width(), im.height(), im.channels());
		for (int x = 0; x < im.width(); x++) {
			for (int y = 0; y < im.height(); y++) {
				newImage(x,y,0) = 0.299 * im(x,y,0) + 0.587 * im(x,y,1) + 0.114 * im(x,y,2);
				newImage(x,y,1) = -0.147 * im(x,y,0) + -0.289 * im(x,y,1) + 0.436 * im(x,y,2);
				newImage(x,y,2) = 0.615 * im(x,y,0) + -0.515 * im(x,y,1) + -0.100 * im(x,y,2);
			}
		}
    return newImage;
}


Image yuv2rgb(const Image &im) {
    // --------- HANDOUT  PS01 ------------------------------
    // Create output image of appropriate size
    // Change colorspace
		Image newImage = Image(im.width(), im.height(), im.channels());
		for (int x = 0; x < im.width(); x++) {
			for (int y = 0; y < im.height(); y++) {
				newImage(x,y,0) = 1.0 * im(x,y,0) + 0.0 * im(x,y,1) + 1.14 * im(x,y,2);
				newImage(x,y,1) = 1.0 * im(x,y,0) + -0.395 * im(x,y,1) + -0.581 * im(x,y,2);
				newImage(x,y,2) = 1.0 * im(x,y,0) + 2.032 * im(x,y,1) + 0.0 * im(x,y,2);
			}
		}
    return newImage;
}


Image saturate(const Image &im, float factor) {
    // --------- HANDOUT  PS01 ------------------------------
    // Create output image of appropriate size
    // Saturate image
    // return output;
		Image yuvImage = rgb2yuv(im);
		for (int x = 0; x < im.width(); x++) {
			for (int y = 0; y < im.height(); y++) {
				yuvImage(x,y,1) *= factor;
				yuvImage(x,y,2) *= factor;
			}
		}
    return yuv2rgb(yuvImage);
}


// Return two images in a C++ vector
std::vector<Image> spanish(const Image &im) {
    // --------- HANDOUT  PS01 ------------------------------
    // Remember to create the output images and the output vector
    // Push the images onto the vector
    // Do all the required processing
    // Return the vector, color image first
		Image colImage = rgb2yuv(im);
		for (int x = 0; x < im.width(); x++) {
			for (int y = 0; y < im.height(); y++) {
				colImage(x,y,0) = .5;
				colImage(x,y,1) *= -1;
				colImage(x,y,2) *= -1;
			}
		}
		std::vector<Image> images;
		images.push_back(yuv2rgb(colImage));
		images.push_back(color2gray(im));
		for (int i = 0; i < im.channels(); i++) {
			images[0](floor(im.width()/2), floor(im.height()/2), i) = 0.0;
		}
		images[1](floor(im.width()/2), floor(im.height()/2), 0) = 0.0;
	return images; //Change this
}


// White balances an image using the gray world assumption
Image grayworld(const Image & im) {
    // --------- HANDOUT  PS01 ------------------------------
    // Implement automatic white balance by multiplying each channel
    // of the input by a factor such that the three channel of the output image
    // have the same mean value. The mean value of the green channel
    // is taken as reference.
		float avg[] = {0, 0, 0};
		for (int x = 0; x < im.width(); x++) {
			for (int y = 0; y < im.height(); y++) {
				for (int c = 0; c < im.channels(); c++) {
					avg[c] += im(x,y,c);
				}
			}
		}

		for (int c = 0; c < im.channels(); c++) {
			avg[c] /= im.width() * im.height();
		}

		float rFactor = avg[1] / avg[0];
		float bFactor = avg[1] / avg[2];

		Image newImage = Image(im.width(), im.height(), im.channels());
		for (int x = 0; x < im.width(); x++) {
			for (int y = 0; y < im.height(); y++) {
				newImage(x,y,0) = im(x,y,0) * rFactor;
				newImage(x,y,1) = im(x,y,1);
				newImage(x,y,2) = im(x,y,2) * bFactor;
			}
		}
    return newImage;
}
