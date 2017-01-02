// hdr.cpp
// Assignment 5


#include "hdr.h"
#include "filtering.h"
#include <math.h>
#include <algorithm>


using namespace std;

/**************************************************************
 //                       HDR MERGING                        //
 *************************************************************/

Image computeWeight(const Image &im, float epsilonMini, float epsilonMaxi){
    // --------- HANDOUT  PS04 ------------------------------
    // Generate a weight image that indicates which pixels are good to use in
    // HDR, i.e. weight=1 when the pixel value is in [epsilonMini, epsilonMaxi].
    // The weight is per pixel, per channel.
    Image newImage(im.width(), im.height(), im.channels());
    for (int i = 0; i < im.number_of_elements(); i++) {
      float value = 0.0;
      if (im(i) >= epsilonMini && im(i) <= epsilonMaxi) {
        value = 1.0;
      }
      newImage(i) = value;
    }
    return newImage;
}


float computeFactor(const Image &im1, const Image &w1, const Image &im2, const Image &w2){
    // --------- HANDOUT  PS04 ------------------------------
    // Compute the multiplication factor between a pair of images. This
    // gives us the relative exposure between im1 and im2. It is computed as
    // the median of im2/(im1+eps) for some small eps, taking into account
    // pixels that are valid in both images.
    vector<float> values;
    float eps = pow(10,-10);
    for (int i = 0; i < im1.number_of_elements(); i++) {
      if (w2(i) != 0 && w1(i) != 0) {
        float factor = (w2(i) * im2(i)) / (w1(i) * im1(i) + eps);
        values.push_back(factor);
      }
    }
    std::sort(values.begin(), values.end());

    if(values.size() % 2 == 0)
            return (values[values.size()/2 - 1] + values[values.size()/2]) / 2;
    else
            return values[values.size()/2];
}


Image makeHDR(vector<Image> &imSeq, float epsilonMini, float epsilonMaxi){
    // --------- HANDOUT  PS04 ------------------------------
    // Merge images to make a single hdr image
    // For each image in the sequence, compute the weight map (special cases
    // for the first and last images).
    // Compute the exposure factor for each consecutive pair of image.
    // Write the valid pixel to your hdr output, taking care of rescaling them
    // properly using the factor.
    float width = imSeq[0].width();
    float height = imSeq[0].height();
    float channels = imSeq[0].channels();
    Image newImage(width, height, channels);
    vector<Image> weights;
    vector<float> factors;
    for (int i = 0; i < imSeq.size(); i++) {
      if (i == 0) {
        weights.push_back(computeWeight(imSeq[i], epsilonMini, 1.0));
        factors.push_back(1.0);
      } else if (i == (imSeq.size() - 1)) {
        weights.push_back(computeWeight(imSeq[i], 0.0, epsilonMaxi));
        float factor = computeFactor(imSeq[i-1], weights[i-1], imSeq[i], weights[i]);
        factor *= factors[i-1];
        factors.push_back(factor);
      } else {
        weights.push_back(computeWeight(imSeq[i], epsilonMini, epsilonMaxi));
        float factor = computeFactor(imSeq[i-1], weights[i-1], imSeq[i], weights[i]);
        factor *= factors[i-1];
        factors.push_back(factor);
      }
    }

    for (int j = 0; j < imSeq[0].number_of_elements(); j++) {
      float normFactor = 0.0;
      for (int i = 0; i < imSeq.size(); i++) {
        if (weights[i](j) != 0) {
          normFactor++;
          newImage(j) += imSeq[i](j) / factors[i];
        }
      }
      if (normFactor == 0) {
        newImage(j) = imSeq[0](j);
      } else {
        newImage(j) /= normFactor;
      }
    }
    return newImage;
}

/**************************************************************
 //                      TONE MAPPING                        //
 *************************************************************/


Image toneMap(const Image &im, float targetBase, float detailAmp, bool useBila, float sigmaRange) {
    // --------- HANDOUT  PS04 ------------------------------
    // tone map an hdr image
    // - Split the image into its luminance-chrominance components.
    // - Work in the log10 domain for the luminance
    // -
    Image newImage(im.width(), im.height(), im.channels());
    vector<Image> decomp = lumiChromi(im);
    Image lumLog = log10Image(decomp[0]);
    Image blurred = Image(im.width(), im.height(), im.channels());
    float sigma;
    if (im.width() > im.height()) {
      sigma = im.width() / 50.0;
    } else {
      sigma = im.height() / 50.0;
    }
    if(useBila) {
      blurred = bilateral(lumLog, sigmaRange, sigma, 3.0, true);
    } else {
      blurred = gaussianBlur_2D(lumLog, sigma, 3.0, true);
    }
    Image detail = lumLog - blurred;

    float largeRange = blurred.max() - blurred.min();
    float k = log10(targetBase) / largeRange;
    Image outLog = detailAmp * detail + k * (blurred - blurred.max());

    decomp[0] = exp10Image(outLog);
    newImage = lumiChromi2rgb(decomp);
    return newImage;
}



/*********************************************************************
 *                       Tone mapping helpers                        *
 *********************************************************************/



// image --> log10Image
Image log10Image(const Image &im) {
    // --------- HANDOUT  PS04 ------------------------------
    // Taking a linear image im, transform to log10 scale.
    // To avoid infinity issues, make any 0-valued pixel be equal the the minimum
    // non-zero value. See image_minnonzero(im).
    Image newImage(im.width(), im.height(), im.channels());
    float minValue = log10(image_minnonzero(im));
    for (int i = 0; i < im.number_of_elements(); i++) {
      if (im(i) == 0) {
        newImage(i) = minValue;
      } else {
        newImage(i) = log10(im(i));
      }
    }
    return newImage;
}

// Image --> 10^Image
Image exp10Image(const Image &im) {
    // --------- HANDOUT  PS04 ------------------------------
    // take an image in log10 domain and transform it back to linear domain.
    // see pow(a, b)
    Image newImage(im.width(), im.height(), im.channels());
    for (int i = 0; i < im.number_of_elements(); i++) {
      newImage(i) = pow(10, im(i));
    }
    return newImage;

}

// min non-zero pixel value of image
float image_minnonzero(const Image &im) {
    // --------- HANDOUT  PS04 ------------------------------
    // return the smallest value in the image that is non-zeros (across all
    // channels too)
    return im.min();

}

/*********************************************************************
 *                       Do not edit below                           *
 *********************************************************************/
Image changeGamma(const Image & im, float old_gamma, float new_gamma) {
    // Image output(im.width(), im.height(), im.channels());
    // Figure out what power to take the values of im, to get the values of output
    // return output;
    float exponent = new_gamma/old_gamma;
    Image output = im;
    for (int i = 0 ; i < im.number_of_elements();i++) {
        output(i) = pow(im(i), exponent);
    }
    return output;
}
