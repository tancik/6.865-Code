/* -----------------------------------------------------------------
 * File:    Image.h
 * Created: 2015-08-29
 * -----------------------------------------------------------------
 * The 6.815/6.865 Image class
 * 
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

    // The distance between adjacent values in image_data in a given dimension where
    // width is dimension 0, height is dimension 1 and channel is dimension 2
    int stride(int dim) const { return stride_[dim]; }

    int extent(int dim) const { return dim_values[dim]; } // Size of dimension
    int width()    const { return dim_values[0]; } //Extent of dimension 0
    int height()   const { return dim_values[1]; } //Extent of dimension 1
    int channels() const { return dim_values[2]; } //Extent of dimension 2

    // Write an image to a file. 
    void write(const std::string & filename) const;
    void debug_write() const; // Writes image to Output directory with automatically chosen name

    static int debugWriteNumber; // Image number for debug write

// The "private" section contains functions and variables that cannot be
// accessed from outside the class.
private:
    unsigned int dims;          // Number of dimensions
    unsigned int dim_values[3]; // Size of each dimension
    unsigned int stride_[3];    // strides
    std::string image_name;     // Image name, will be the filename if read from a file

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

#endif
