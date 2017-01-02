/* --------------------------------------------------------------------------
 * File:    a2_main.cpp
 * Created: 2015-09-23
 * --------------------------------------------------------------------------
 *
 *
 *
 * ------------------------------------------------------------------------*/


#include "Image.h"
#include "filtering.h"
#include <ctime>
#include <iostream>
#include <vector>

using namespace std;


// This is a way for you to test your functions.
// We will only grade the contents of filter.cpp and Image.cpp
int main() {

    // // ------- Example tests, change them ! --------------
    // Image im = impulseImg(10);
    // cout << "smart accessor at (1,3,0): " << im.smartAccessor(1,3,0,true) << endl;
    // Image blurred = boxBlur(im, 3, true);
    // cout << "blurred impulse image" << endl;
    // blurred.write("./Output/blur.png");
    // Image image("./Input/bear.png");
    // Image blur = boxBlur(image, 9, false);
    // blur.write("./Output/blurredBear.png");
    // cout << "keep testing..." << endl;
    // // ---------------------------------------------------
    //
    //
    // // ---------------------------------------------------
    // // Test the filter class on an impulse image
    // Image dirac = impulseImg(31);
    //
    // // Test kernel
    // vector<float> kernel{1,0,0,
    //                      0,1,0,
    //                      1,0,0}; // C++11 syntax
    // Filter testFilter(kernel, 3, 3);
    // Image testOutput = testFilter.convolve(dirac);
    // // The output should be an exact copy of the kernel in the center of the
    // // image
    // testOutput.write("./Output/testKernel.png");
    //
    // Image boxImg = impulseImg(10);
    // Image boxFilterBlur = boxBlur_filterClass(boxImg, 3, true);
    // boxFilterBlur.write("./Output/blur2.png");
    // // ---------------------------------------------------
    //
    //
    // // ---------------------------------------------------
    // // E.g. test the sobelKernel
    // // create Sobel Filter that extracts horizontal gradients
    // // [ -1 0 1 ]
    // // [ -2 0 2 ]
    // // [ -1 0 1 ]
    // float fDataXArray[] = { -1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0 };
    // vector<float> fDataX (fDataXArray, fDataXArray + sizeof(fDataXArray) / sizeof(float) );
    // Filter sobelX(fDataX, 3, 3);
    //
    // // verify that your filter is correct by using it to filter an impulse image
    // Image impulse = impulseImg(11); //create an image containing an impulse
    // // convolve the impulse image with the Sobel kernel. We divide the output by 4 and
    // // add 0.5 to make the range of the image between 0 and 1
    // Image verifyKernel = sobelX.convolve(impulse)/4 + 0.5;
    // verifyKernel.write("./Output/verifySobelKernel.png");
    //
    // // filter an image using the sobel kernel
    // Image im2("./Input/lounge_view.png");
    // Image sobelFiltered = sobelX.convolve(im2);
    //
    // // make the range of the output image from 0 to 1 for visualization
    // // since the Sobel filter changes the range of a (0,1) image to (-2,2)
    // Image sobelOut = sobelFiltered/4 + 0.5;
    // sobelOut.write("./Output/sobelFiltered.png");
    //
    // Image lounge("./Input/lounge_view.png");
    // Image grad = gradientMagnitude(lounge, true);
    // grad.write("./Output/grad.png");
    // // ---------------------------------------------------
    //
    // // Image cambridge = impulseImg(20);
    // // Image cambridge("./Input/Cambridge1.png");
    // // Image out = gaussianBlur_horizontal(cambridge, 2, 2, true);
    // // out.write("./Output/1dblur.png");
    // //
    // // Image cambridge("./Input/Cambridge1.png");
    // // Image out = gaussianBlur_2D(cambridge, 2, 2, true);
    // // out.write("./Output/2dblur.png");
    // // Image loungeSharp = unsharpMask(lounge, 3, 2, .3, true);
    // // loungeSharp.write("./Output/loungeSharpen.png");
    //     // --------- HANDOUT  PS02 ------------------------------
    //
    //
    // // --- Timer example ---------------------------------
    // // clock_t start = clock();
    // // float sigma = 2.0f;
    // // Image blurHorizontal = gaussianBlur_2D(im2, sigma);
    // // clock_t end = clock();
    // // double duration = (end-start)*1.0f/CLOCKS_PER_SEC;
    // // cout << "2D gaussian took: " << duration <<"s" << endl;
    // // ---------------------------------------------------
    //
    // Image noise("./Input/lens.png");
    // Image noNoise = bilaYUV(noise, .1, 1.0, 6.0, 3, true);
    // Image noNoise2 = bilateral(noise, .1, 1.0, 3, true);
    // noNoise.write("./Output/lensNoNoise.png");
    // noNoise2.write("./Output/lenNoNoise2.png");

    Image camb1("./Input/Cambridge1.png");
    Image camb2("./Input/Cambridge2.png");
    Image bear("./Input/bear.png");
    Image lens("./Input/lens.png");
    Image apple("./Input/apple.png");
    Image im = impulseImg(20);

    Image median = medianFilter(apple, 5, true);
    median.write("./Output/median.png");
    Image denoise_1 = bilateral(lens, 0.1, 1.0, 3.0, true);
    denoise_1.write("./Output/denoise_1.png");
    Image denoise_2 = bilaYUV(lens, 0.1, 1.0, 4.0, 3.0, true);
    denoise_2.write("./Output/denoise_2.png");
}
