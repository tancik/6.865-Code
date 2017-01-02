/* -----------------------------------------------------------------
 * File:    a3_main.cpp
 * Author:  Michael Gharbi <gharbi@mit.edu>
 * Created: 2015-09-30
 * -----------------------------------------------------------------
 *
 *
 *
 * ---------------------------------------------------------------*/


#include "Image.h"
#include "basicImageManipulation.h"
#include "demosaic.h"
#include "align.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>


using namespace std;


// This is a way for you to test your functions.
// We will only grade the contents of demosaic.cpp and align.cpp
int main() {
    cout << "nothing done in a5_main.cpp, debug me !" << endl;
    //Denoise ---------------------------
    //Load sequence
    // vector<Image> seq;
    // int n_images = 16;
    // for (int i = 1; i <= n_images; ++i) {
    //     ostringstream fname;
    //     fname << "./Input/aligned-ISO400/1D2N-iso400-under-";
    //     // fname << "./Input/aligned-ISO3200/1D2N-iso3200-";
    //     fname << i;
    //     fname << ".png";
    //     seq.push_back(Image(fname.str()));
    // }
    //
    // // Denoise
    // Image out = denoiseSeq(seq);
    // out.write("./Output/denoised.png");
    //
    // Image SNRIm = logSNR(seq,1/30.0);
    // SNRIm.write("./Output/snr_map_400.png");

    // vector<Image> seqSmall;
    // int n_small_images = 4;
    // for (int i = 1; i <= n_small_images; ++i) {
    //     ostringstream fname;
    //     fname << "./Input/Align/test-";
    //     // fname << "./Input/aligned-ISO3200/1D2N-iso3200-";
    //     fname << i;
    //     fname << ".png";
    //     seqSmall.push_back(Image(fname.str()));
    // }
    // Image alignImgs = alignAndDenoise(seqSmall,8);
    // alignImgs.write("./Output/alignedTest.png");

    // Image denoise = denoiseSeq(seq);
    // denoise.write("./Output/green_denoised.png");
    // Image alignedDenoise = alignAndDenoise(seq, 20);
    // alignedDenoise.write("./Output/green_denoised_aligned.png");

    // vector<Image> seq;
    // int n_images = 3;
    // for (int i = 1; i <= n_images; ++i) {
    //     ostringstream fname;
    //     fname << "./Input/green/noise-small-";
    //     // fname << "./Input/aligned-ISO3200/1D2N-iso3200-";
    //     fname << i;
    //     fname << ".png";
    //     seq.push_back(Image(fname.str()));
    // }
    // Image denoise = denoiseSeq(seq);
    // denoise.write("./Output/green_denoised.png");
    // Image alignedDenoise = alignAndDenoise(seq, 5);
    // alignedDenoise.write("./Output/green_denoised_aligned.png");

    // Demosaic ---------------------------
    // Image raw("./Input/raw/signs-small.png");
    // Image green = basicGreen(raw, 1);
    // green.write("./Output/demosaic_green.png");
    // Image red = basicRorB(raw, 1, 1);
    // red.write("./Output/demosaic_red.png");
    // Image blue = basicRorB(raw, 0, 0);
    // blue.write("./Output/demosaic_blue.png");
    // Image rgb = basicDemosaic(raw, 1, 1,1,0,0);
    // rgb.write("./Output/demosaiced.png");
    // Image rgb_green_adv = edgeBasedGreenDemosaic(raw, 1, 1,1,0,0);
    // rgb_green_adv.write("./Output/demosaiced_edge_green.png");
    // Image rgb_all_adv = improvedDemosaic(raw, 1, 1, 1, 0, 0);
    // rgb_all_adv.write("./Output/demosaiced_best.png");
    // Image test("./Input/raw/test.png");
    // Image test_green = edgeBasedGreen(test, 0);
    // test.write("./Output/what.png");
    // test_green.write("./Output/test_green.png");

    //
    // Sergey ---------------------------
    Image sergeyImg("./Input/Sergey/00088v_third.png");
    Image rgb2 = split(sergeyImg);
    rgb2.write("./Output/Sergey_split.png");
    Image rgbAlign = sergeyRGB(sergeyImg,10);
    rgbAlign.write("./Output/Sergey_aligned.png");

    return 0;
}
