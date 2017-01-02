#include "matrix.h"
#include "homography.h"
#include <iostream>
#include <cassert>

using namespace std;

void testStitchScience() {
    // // --------- HANDOUT  PS06 ------------------------------
    // 6.865 write a test
    // test boston
    Image monu1 = Image("Input/science-1.png");
    Image monu2 = Image("Input/science-2.png");
    CorrespondencePair corresp[4] = {
      CorrespondencePair( 10,294,1, 210,289,1),
      CorrespondencePair( 46,189,1, 235,182,1),
      CorrespondencePair( 101,192,1, 291,179,1),
      CorrespondencePair( 84,366,1, 288,357,1)
    };
    Image monu = stitch(monu1, monu2, corresp);
    monu.write("./Output/science-stitch.png");
}

void testStitchConvention() {
    // // --------- HANDOUT  PS06 ------------------------------
    // 6.865 write a test
    Image monu1 = Image("Input/convention-1.png");
    Image monu2 = Image("Input/convention-2.png");
    CorrespondencePair corresp[4] = {
      CorrespondencePair( 172,300,1, 31,319,1),
      CorrespondencePair( 316,285,1, 173,288,1),
      CorrespondencePair( 194,183,1, 50,191,1),
      CorrespondencePair( 264,134,1, 118,143,1)
    };
    Image monu = stitch(monu1, monu2, corresp);
    monu.write("./Output/convention-stitch.png");
}

void testStitchBoston1() {
    // // --------- HANDOUT  PS06 ------------------------------
    // 6.865 write a test
    Image monu1 = Image("Input/boston1-1.png");
    Image monu2 = Image("Input/boston1-2.png");
    CorrespondencePair corresp[4] = {
      CorrespondencePair( 190,304,1, 32,310,1),
      CorrespondencePair( 217,91,1, 62,87,1),
      CorrespondencePair( 271,163,1, 115,166,1),
      CorrespondencePair( 270,254,1, 116,257,1)
    };
    Image monu = stitch(monu1, monu2, corresp);
    monu.write("./Output/boston1-stitch.png");
}
