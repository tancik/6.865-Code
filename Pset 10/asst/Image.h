/* -----------------------------------------------------------------
 * File:    Image.h
 * Created: 2015-08-29
 * -----------------------------------------------------------------
 * 
 * The 6.815/6.865 Image class
 * 
 * ---------------------------------------------------------------*/


#ifndef __IMAGE__H
#define __IMAGE__H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cfloat>
#include <cmath>

#include "ImageException.h"
#include "lodepng.h"

class Image {
public:
    // Constructor to initialize an image of size width_*height_*channels_
    // If height_ and channels_ are zero, the image will be one dimensional
    // If channels_ is zero, the image will be two dimensional
    Image(int width_, int height_ = 0, int channels_ = 0,  const std::string &name="");

    // Constructor to create an image from a file. The file needs to be in the PNG format
    Image(const std::string & filename);

    // Destructor. Because there is no explicit memory management here, this doesn't do anything
    ~Image();

    // Returns the images name, should you specify one
    const std::string & name() const { return image_name; }

    // The number of dimensions in the image (1, 2 or 3)
    int dimensions() const { return dims; }

    // --------- HANDOUT  PS01 ------------------------------
    float gamma() const { return gamma_; } // Gamma of image
    // ------------------------------------------------------

    // The distance between adjacent values in image_data in a given dimension where
    // width is dimension 0, height is dimension 1 and channel is dimension 2
    int stride(int dim) const { return stride_[dim]; }

    int width()    const { return dim_values[0]; } //Extent of dimension 0
    int height()   const { return dim_values[1]; } //Extent of dimension 1
    int channels() const { return dim_values[2]; } //Extent of dimension 2

    int extent(int dim) const { return dim_values[dim]; } // Size of dimension

    // Write an image to a file. 
    void write(const std::string & filename) const;
    void debug_write() const; // Writes image to Output directory with automatically chosen name

    // --------- HANDOUT  PS01 ------------------------------
    // The total number of elements. Should be equal to width()*height()*channels()
    // That is, a 200x100x3 image has 60000 pixels not 20000 pixels
    long long number_of_elements() const;

    // Accessors for the pixel values
    const float & operator()(int x) const;
    const float & operator()(int x, int y) const;
    const float & operator()(int x, int y, int z) const;
    
    // Setters for the pixel values. A reference to the value in image_data is returned
    float & operator()(int x);
    float & operator()(int x, int y);
    float & operator()(int x, int y, int z);
    // ------------------------------------------------------

    static int debugWriteNumber; // Image number for debug write

    // --------- HANDOUT  PS02 ------------------------------
    // Safe Accessor that will return a black pixel (clamp = false) or the
    // nearest pixel value (clamp = true) when indexing out of the bounds of the image
    float smartAccessor(int x, int y, int z, bool clamp=false) const;
    // ------------------------------------------------------

    // --------- HANDOUT  PS07 ------------------------------
    float min() const;
    float max() const;
    float mean() const;
    float var() const;

    // ------------------------------------------------------

// The "private" section contains functions and variables that cannot be
// accessed from outside the class.
private:
    unsigned int dims;          // Number of dimensions
    unsigned int dim_values[3]; // Size of each dimension
    unsigned int stride_[3];    // strides
    std::string image_name;     // Image name, will be the filename if read from a file

    // --------- HANDOUT  PS01 ------------------------------
    float gamma_; // Gamma factor, most images are roughly 2.2
    // ------------------------------------------------------

    // This vector stores the values of the pixels. A vector in C++ is an array
    // that manages its own memory
    std::vector<float> image_data;

    // Helper functions for reading and writing
    static float uint8_to_float(const unsigned char &in); // Converts uint8 to float, 255 -> 1, 0 -> 0
    static unsigned char float_to_uint8(const float &in); // Converts floats to uint8 0 -> 0, 1 -> 255

    // Common code shared between constructors
    // This does not allocate the image; it only initializes image metadata -
    // image name, width, height, number of channels and number of pixels
    void initialize_image_metadata(int x, int y, int z, const std::string &name_);
};

// --------- HANDOUT  PS01 ------------------------------
// NOTE: provided for pset01
void compareDimensions(const Image & im1, const Image & im2);

// Image/Image element-wise operations
// NOTE: provided for pset01
Image operator+ (const Image & im1, const Image & im2);
Image operator- (const Image & im1, const Image & im2);
Image operator* (const Image & im1, const Image & im2);
Image operator/ (const Image & im1, const Image & im2);

// Image/scalar operations 
// NOTE: provided for pset01
Image operator+ (const Image & im1, const float & c);
Image operator- (const Image & im1, const float & c);
Image operator* (const Image & im1, const float & c);
Image operator/ (const Image & im1, const float & c);

// scalar/Image operations 
// NOTE: provided for pset01
Image operator+ (const float & c, const Image & im1);
Image operator- (const float & c, const Image & im1);
Image operator* (const float & c, const Image & im1);
Image operator/ (const float & c, const Image & im1);
// ------------------------------------------------------

#endif
