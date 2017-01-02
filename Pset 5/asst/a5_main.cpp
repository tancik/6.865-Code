/* -----------------------------------------------------------------
 * File:    a5_main.cpp
 * Author:  Michael Gharbi <gharbi@mit.edu>
 * Created: 2015-09-30
 * -----------------------------------------------------------------
 *
 *
 *
 * ---------------------------------------------------------------*/


#include "Image.h"
#include "basicImageManipulation.h"
#include "morphing.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace std;

void testNearestNeighbor() {
    // Test nearest neighbor
    const Image im("./Input/BostonRainbow-crop-400.png");
    float fact = 3.5;
    Image scaledNN = scaleNN(im, fact);
    scaledNN.write("./Output/testNN.png");
}

void testBilinearInterpolation() {
    // Test bilinear interpolation
    cout << endl << "linear interpolation" << endl;
    cout <<  "--------------------" << endl;
    Image test(2,2,1);
    test(0,0,0) = 0.0f;
    test(0,1,0) = 0.25f;
    test(1,0,0) = 0.5f;
    test(1,1,0) = 1.0f;

    float x = 0.25f,
          y = 0.0;
    cout << "interpolate at (" << x << ", " << y << "): "
         << interpolateLin(test,x,y,0,false)
         << ", should be 0.125"
         << endl;

    x = 0.0;
    y = 0.25f;
    cout << "interpolate at (" << x << ", " << y << "): "
         << interpolateLin(test,x,y,0,false)
         << ", should be 0.0625"
         << endl;

    x = 0.5;
    y = 0.5f;
    cout << "interpolate at (" << x << ", " << y << "): "
         << interpolateLin(test,x,y,0,false)
         << ", should be 0.4375"
         << endl;
}

void testBilinearRescaling() {
    // Test bilinear
    const Image im("./Input/BostonRainbow-crop-400.png");
    float fact = 3.5;
    Image scaled= scaleLin(im, fact);
    scaled.write("./Output/testLin.png");
}

void testRotation() {
    const Image im("./Input/BostonRainbow-crop-400.png");

    float theta = M_PI/4;

    Image rot = rotate(im, theta);
    rot.write("./Output/testRotate.png");
}

void testVectorOperations() {
    // Test vector lib
    Vec2f a(10.0f,3.0f);
    Vec2f b(-4.1f,2.7f);
    float f = 2.0f;
    cout << endl << "vector operations" << endl;
    cout << "-----------------" << endl ;

    cout << "a(" << a.x << ", " << a.y << ")" << endl;
    cout << "b(" << b.x << ", " << b.y << ")" << endl;
    cout << "f=" << f << endl;

    // a+b
    Vec2f c = add(a,b);
    cout << "a+b: (" << c.x << ", " << c.y << ")" << endl;

    // a-b
    c = subtract(a,b);
    cout << "a-b: (" << c.x << ", " << c.y << ")" << endl;

    // a*f
    c = scalarMult(a,f);
    cout << "a*f: (" << c.x << ", " << c.y << ")" << endl;

    // <a,b>
    float s = dot(a,b);
    cout << "<a,b>=" << s << endl;

    // ||a||
    float l = length(a);
    cout << "||a||=" << l << endl;
}

void testSegment() {
    // Test segment
    // are P,Q,e1 e1 correctly implemented?
    // are the u,v coordinates meaningful?
    // What should be u and v for P,Q ?
    // Come up with a few test cases !

}

void testWarpBy1() {
    // Test warpBy1 ----------------------------------
    Image fredo("./Input/test.png");
    Image bear("./Input/bear.png");

    // define before and after segments
    Segment segBefore(Vec2f(0, 0), Vec2f(1,1));
    Segment segAfter(Vec2f(0, 0), Vec2f(1,1));

    // warp
    Image warp1 = warpBy1(fredo, segBefore, segAfter);
    warp1.write("./Output/test_new.png");

    Image bearWarp = warpBy1(bear, Segment(Vec2f(0,0), Vec2f(10,0)), Segment(Vec2f(10, 10), Vec2f(30, 15)));
    bearWarp.write("./Output/bearWarp.png");
    // ------------------------------------------------
}

void testWarpBy1_2() {
    // Test warpBy1 ----------------------------------
    Image fredo("./Input/test2.png");

    // define before and after segments
    Segment segBefore(Segment(Vec2f(74, 126), Vec2f(125, 74)));
    Segment segAfter(Segment(Vec2f(83, 137), Vec2f(121, 49)));

    // warp
    Image warp1 = warpBy1(fredo, segBefore, segAfter);
    warp1.write("./Output/test_single.png");
    // ------------------------------------------------
}

void testDistance() {
    Image newImage(200,200,1);
    Segment seg(Vec2f(170, 40), Vec2f(90, 100));
    float max = 0;
    for (int x= 0; x < newImage.width(); x ++) {
      for (int y = 0; y < newImage.height(); y++) {
        float val = seg.distance(Vec2f(x,y));
        if (val > max) {
          max = val;
        }
        newImage(x,y,0) = val;
      }
    }
    newImage = newImage / max;
    newImage.write("./Output/distanceTest.png");
}

void testWarp() {
    // Make your own test !
    Image fredo("./Input/test2.png");
    Image bear("./Input/bear.png");

    vector<Segment> segsBefore;
    segsBefore.push_back(Segment(Vec2f(74, 126), Vec2f(125, 74)));
    segsBefore.push_back(Segment(Vec2f(78, 174), Vec2f(81, 188)));
    segsBefore.push_back(Segment(Vec2f(146, 19), Vec2f(165, 87)));
    segsBefore.push_back(Segment(Vec2f(29, 57), Vec2f(65, 54)));
    vector<Segment> segsAfter;
    segsAfter.push_back(Segment(Vec2f(83, 137), Vec2f(121, 49)));
    segsAfter.push_back(Segment(Vec2f(137, 171), Vec2f(142, 189)));
    segsAfter.push_back(Segment(Vec2f(121, 9), Vec2f(190, 31)));
    segsAfter.push_back(Segment(Vec2f(7, 87), Vec2f(42, 86)));

    // warp
    Image warp1 = warp(fredo, segsBefore, segsAfter, 10.0, 1.0, 1.0);
    warp1.write("./Output/test_double.png");
}

void testMorph() {
    // Test morph
    Image fredo("./Input/fredo2.png");
    Image werewolf("./Input/werewolf.png");

    // ... use the UI to specify segments
    vector<Segment> segsBefore;
    segsBefore.push_back(Segment(Vec2f(85, 102), Vec2f(119, 108)));
    segsBefore.push_back(Segment(Vec2f(143, 117), Vec2f(169, 114)));
    vector<Segment> segsAfter;
    segsAfter.push_back(Segment(Vec2f(86, 93), Vec2f(114, 104)));
    segsAfter.push_back(Segment(Vec2f(137, 94), Vec2f(152, 79)));

    // This should monsterify Fredo a little.
    vector<Image> out = morph(fredo, werewolf,  segsBefore, segsAfter, 5);

    // This is how you can write an image sequence
    int i = 0;
    for (; i < out.size(); ++i) {
        ostringstream fname;
        fname << "./Output/test_morph_";
        fname << setfill('0') << setw(2);
        fname << i;
        fname << ".png";
        out[i].write(fname.str());
    }
}

void testMorph2() {
    // Test morph
    Image personOne("./Input/matt.png");
    Image personTwo("./Input/hannah.png");

    // Test morph
    vector<Segment> segsBefore;
    segsBefore.push_back(Segment(Vec2f(101, 168), Vec2f(132, 168)));
    segsBefore.push_back(Segment(Vec2f(172, 170), Vec2f(202, 172)));
    segsBefore.push_back(Segment(Vec2f(112, 236), Vec2f(149, 238)));
    segsBefore.push_back(Segment(Vec2f(149, 252), Vec2f(184, 239)));
    segsBefore.push_back(Segment(Vec2f(179, 203), Vec2f(195, 227)));
    segsBefore.push_back(Segment(Vec2f(128, 208), Vec2f(174, 208)));
    segsBefore.push_back(Segment(Vec2f(134, 47), Vec2f(192, 54)));
    segsBefore.push_back(Segment(Vec2f(218, 189), Vec2f(210, 232)));
    segsBefore.push_back(Segment(Vec2f(84, 231), Vec2f(134, 302)));
    segsBefore.push_back(Segment(Vec2f(152, 166), Vec2f(152, 218)));
    segsBefore.push_back(Segment(Vec2f(134, 151), Vec2f(106, 144)));
    segsBefore.push_back(Segment(Vec2f(176, 148), Vec2f(203, 150)));
    segsBefore.push_back(Segment(Vec2f(89, 137), Vec2f(84, 186)));
    segsBefore.push_back(Segment(Vec2f(191, 87), Vec2f(213, 113)));
    segsBefore.push_back(Segment(Vec2f(200, 256), Vec2f(163, 297)));
    vector<Segment> segsAfter;
    segsAfter.push_back(Segment(Vec2f(99, 206), Vec2f(124, 204)));
    segsAfter.push_back(Segment(Vec2f(164, 207), Vec2f(191, 208)));
    segsAfter.push_back(Segment(Vec2f(114, 263), Vec2f(141, 265)));
    segsAfter.push_back(Segment(Vec2f(144, 280), Vec2f(174, 264)));
    segsAfter.push_back(Segment(Vec2f(169, 238), Vec2f(181, 262)));
    segsAfter.push_back(Segment(Vec2f(124, 240), Vec2f(163, 238)));
    segsAfter.push_back(Segment(Vec2f(123, 107), Vec2f(179, 112)));
    segsAfter.push_back(Segment(Vec2f(216, 216), Vec2f(207, 272)));
    segsAfter.push_back(Segment(Vec2f(82, 270), Vec2f(124, 323)));
    segsAfter.push_back(Segment(Vec2f(143, 201), Vec2f(143, 246)));
    segsAfter.push_back(Segment(Vec2f(128, 184), Vec2f(102, 178)));
    segsAfter.push_back(Segment(Vec2f(164, 185), Vec2f(194, 185)));
    segsAfter.push_back(Segment(Vec2f(86, 174), Vec2f(78, 222)));
    segsAfter.push_back(Segment(Vec2f(181, 135), Vec2f(205, 157)));
    segsAfter.push_back(Segment(Vec2f(194, 293), Vec2f(160, 322)));

    // This should monsterify Fredo a little.
    vector<Image> out = morph(personOne, personTwo,  segsBefore, segsAfter, 28);

    // This is how you can write an image sequence
    int i = 0;
    for (; i < out.size(); ++i) {
        ostringstream fname;
        fname << "./Output/morphs/matt_hannah";
        fname << setfill('0') << setw(2);
        fname << i+1;
        fname << ".png";
        out[i].write(fname.str());
    }
}


// This is a way for you to test your functions.
// We will only grade the contents of morphing.cpp and basicImageManipulation.cpp
int main() {
    // testNearestNeighbor();
    // testBilinearInterpolation();
    // testBilinearRescaling();
    // testRotation();
    // testVectorOperations();
    // testSegment();
    // testWarpBy1();
    // testWarpBy1_2();
    // testDistance();
    // testWarp();
    // testMorph();
    testMorph2();
}
