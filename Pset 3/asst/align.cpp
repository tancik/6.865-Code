/* --------------------------------------------------------------------------
 * File:    align.cpp
 * Created: 2015-10-01
 * --------------------------------------------------------------------------
 *
 *
 *
 * ------------------------------------------------------------------------*/



#include "align.h"

using namespace std;

Image denoiseSeq(const vector<Image> &imSeq){
    // // --------- HANDOUT  PS03 ------------------------------
    // Basic denoising by computing the average of a sequence of images
    int width = imSeq[0].width();
    int height = imSeq[0].height();
    int channels = imSeq[0].channels();
    Image newImage(width, height, channels);
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        for (int c = 0; c < channels; c++) {
          for (int i = 0; i < imSeq.size(); i++) {
            newImage(x,y,c) += imSeq[i](x,y,c);
          }
        }
      }
    }
    return newImage / imSeq.size();

}


Image logSNR(const vector<Image> &imSeq, float scale){
    // // --------- HANDOUT  PS03 ------------------------------
    // returns an image visualizing the per-pixel and
    // per-channel log of the signal-to-noise ratio scaled by scale.
    float n = imSeq.size();
    float width = imSeq[0].width();
    float height = imSeq[0].height();
    float channels = imSeq[0].channels();
    Image newImage(width, height, channels);
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        for (int c = 0; c < channels; c++) {
          float EI = 0;
          float EI2 = 0;
          for (int i = 0; i < n; i++) {
            float lum = imSeq[i](x,y,c);
            EI += lum;
            EI2 += pow(lum, 2);
          }
          EI /= n;
          EI2 /= n;

          float sigma = 0;
          for (int i = 0; i < n; i++) {
            sigma += pow(EI - imSeq[i](x,y,c), 2);
          }
          sigma /= (n - 1);
          if (sigma == 0) {
            sigma = .0000001;
          }
          newImage(x,y,c) = scale * 10 * log10(EI2 / sigma);
        }
      }
    }
    return newImage;
}


vector<int> align(const Image &im1, const Image &im2, int maxOffset){
    // // --------- HANDOUT  PS03 ------------------------------
    // returns the (x,y) offset that best aligns im2 to match im1.
    int bestx = 0;
    int besty = 0;
    float bestDifference = -1;
    for (int dx = -maxOffset; dx < maxOffset + 1; dx++) {
      for (int dy = -maxOffset; dy < maxOffset + 1; dy++) {
        float difference = 0;
        for (int y = maxOffset; y < im1.height() - maxOffset; y++) {
          for (int x = maxOffset; x < im1.width() - maxOffset; x++) {
            for (int c = 0; c < im1.channels(); c++) {
              difference += pow(im1(x,y,c) - im2(x+dx,y+dy,c), 2);
            }
          }
        }
        if (difference < bestDifference || bestDifference < 0) {
            bestDifference = difference;
            bestx = dx;
            besty = dy;
        }
      }
    }
    vector<int> delta;
    delta.push_back(-bestx);
    delta.push_back(-besty);
    return delta;
}

Image alignAndDenoise(const vector<Image> &imSeq, int maxOffset){
    // // --------- HANDOUT  PS03 ------------------------------
    // Registers all images to the first one in a sequence and outputs
    // a denoised image even when the input sequence is not perfectly aligned.
    vector<Image> alignedImgSeq;
    alignedImgSeq.push_back(imSeq[0]);
    for (int i = 1; i < imSeq.size(); i++) {
      vector<int> delta = align(imSeq[0], imSeq[i], maxOffset);
      alignedImgSeq.push_back(roll(imSeq[i], delta[0], delta[1]));
    }
    return denoiseSeq(alignedImgSeq);
}

Image split(const Image &sergeyImg){
    // --------- HANDOUT  PS03 ------------------------------
    // 6.865 only:
    // split a Sergey images to turn it into one 3-channel image.
    int width = floor(sergeyImg.width());
    int height = floor(sergeyImg.height() / 3);
    Image newImage(width, height, 3);
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        newImage(x,y,2) = sergeyImg(x + y * width);
        newImage(x,y,1) = sergeyImg(x + y * width + width * height);
        newImage(x,y,0) = sergeyImg(x + y * width + 2 * width * height);
      }
    }
    return newImage;
}

Image sergeyRGB(const Image &sergeyImg, int maxOffset){
    // // --------- HANDOUT  PS03 ------------------------------
    // 6.865 only:
    // aligns the green and blue channels of your rgb channel of a sergey
    // image to the red channel. This should return an aligned RGB image
    Image splitImg = split(sergeyImg);
    Image newImage(splitImg.width(), splitImg.height(), 3);
    Image red(splitImg.width(), splitImg.height(), 1);
    Image green(splitImg.width(), splitImg.height(), 1);
    Image blue(splitImg.width(), splitImg.height(), 1);

    for (int y = 0; y < splitImg.height(); y++) {
      for (int x = 0; x < splitImg.width(); x++) {
        red(x,y) = splitImg(x,y,0);
        green(x,y) = splitImg(x,y,1);
        blue(x,y) = splitImg(x,y,2);
      }
    }
    vector<int> deltaBlue = align(red, blue, maxOffset);
    vector<int> deltaGreen = align(red, green, maxOffset);
    Image alignedBlue = roll(blue, deltaBlue[0], deltaBlue[1]);
    Image alignedGreen = roll(green, deltaGreen[0], deltaGreen[1]);

    for (int y = 0; y < splitImg.height(); y++) {
      for (int x = 0; x < splitImg.width(); x++) {
        newImage(x,y,0) = red(x,y);
        newImage(x,y,1) = alignedGreen(x,y);
        newImage(x,y,2) = alignedBlue(x,y);
      }
    }
    return newImage;
}


/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// This circularly shifts an image by xRoll in the x direction and
// yRoll in the y direction. xRoll and yRoll can be negative or postive
Image roll(const Image &im, int xRoll, int yRoll){

    int xNew, yNew;
    Image imRoll(im.width(), im.height(), im.channels());

    // for each pixel in the original image find where its corresponding
    // location is in the rolled image
    for (int x=0; x<im.width(); x++){
        for (int y=0; y<im.height(); y++){

            // use modulo to figure out where the new location is in the
            // rolled image. Then take care of when this returns a negative number
            xNew = (x + xRoll) % im.width();
            yNew = (y + yRoll) % im.height();
            xNew = (xNew<0)*(imRoll.width() + xNew) + (xNew>=0)*xNew;
            yNew = (yNew<0)*(imRoll.height() + yNew) + (yNew>=0)*yNew;

            // assign the rgb values for each pixel in the original image to
            // the location in the new image
            for (int z=0; z<im.channels(); z++){
                imRoll(xNew, yNew, z) = im(x,y,z);
            }
        }
    }

    // return the rolled image
    return imRoll;
}
