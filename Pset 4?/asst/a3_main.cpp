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
#include "morphing.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>


using namespace std;


// This is a way for you to test your functions.
// We will only grade the contents of morphing.cpp and basicImageManipulation.cpp
int main() {
    cout << "nothing done in a3_main.cpp, debug me !" << endl;
    // // // Test nearest neighbor
    // const Image im("./Input/BostonRainbow-crop-400.png");
    // float fact = 3;
    // Image scaledNN = scaleNN(im, fact);
    // scaledNN.write("./Output/scaledNN.png");

    // // Test bilinear interpolation
    // Image test(2,2,1);
    // test(0,0,0) = 0.0f;
    // test(0,1,0) = 1.0f;
    // test(1,0,0) = 0.0f;
    // test(1,1,0) = 1.0f;
    //
    // float x = 0.0f,
    //       y = 1.0f;
    // cout << "interpolate at (" << x << ", " << y << ")"
    //      << endl
    //      << interpolateLin(test,x,y,0,false);

    // Test bilinear rescaling
    // Image scaledBilinear = scaleLin(im, 4);
    // scaledBilinear.write("./Output/bilinearScale.png");

    // Test rotation (6.865)
    // Image rotation = rotate(im, M_PI/4.0);
    // rotation.write("./Output/rotate.png");

    // Test vector lib
    // Vec2f a(1.0f, 2.0f);
    // Vec2f b(2.0f, 3.0f);
    // float f = 4.0;
    // std::cout << "a=(" << a.x << ", " << a.y << "), b=(" << b.x << ", " << b.y << "), f=" << f << std::endl;
    // // a+b
    // std::cout << "a+b=(" << add(a, b).x << ", " << add(a, b).y << ")" << std::endl;
    // // a-b
    // std::cout << "a-b=(" << subtract(a, b).x << ", " << subtract(a, b).y << ")" << std::endl;
    // // f*b
    // std::cout << "a*f=(" << scalarMult(a, f).x << ", " << scalarMult(a, f).y << ")" << std::endl;
    // // <a,b>
    // std::cout << "a.b=" << dot(a, b) << std::endl;
    // // ||a||^2
    // std::cout << "length(a)=" << length(a) << std::endl;

    // Test segment
    // are P,Q,e1 e1 correctly implemented?
    // are the u,v coordinates meaningful?
    // What should be u and v for P,Q ?
    // Vec2f c(0.0f, 0.0f);
    // Vec2f d(0.0f, 1.0f);
    // Vec2f X(0.5f, 0.8f);
    // float f = 4.0;
    // Segment seg(c,d);
    // std::cout << "c=(" << c.x << ", " << c.y << "), d=(" << d.x << ", " << d.y << "), f=" << f << std::endl;
    // std::cout << "P=(" << seg.getP().x << ", " << seg.getP().y << ")" << std::endl;
    // std::cout << "Q=(" << seg.getQ().x << ", " << seg.getQ().y << ")" << std::endl;
    // std::cout << "E1=(" << seg.getE1().x << ", " << seg.getE1().y << ")" << std::endl;
    // std::cout << "E2=(" << seg.getE2().x << ", " << seg.getE2().y << ")" << std::endl;
    // std::cout << "XtoUV=(" << seg.XtoUV(X).x << ", " << seg.XtoUV(X).y << ")" << std::endl;

    // // Test warpBy1 ----------------------------------
    Image fredo("./Input/fredo2.png");
    Image bear("./Input/bear.png");
    Image test("./Input/test.png");

    // define before and after segments
    Segment segBefore(Vec2f(0, 0), Vec2f(0, 1));
    Segment segAfter(Vec2f(0, 0), Vec2f(1, 0));

    // warp
    Image warp1 = warpBy1(fredo, segBefore, segAfter);
    warp1.write("./Output/fredo_upside_down.png");
    Image bearWarp = warpBy1(bear, Segment(Vec2f(0,0), Vec2f(10,0)), Segment(Vec2f(10, 10) , Vec2f(30, 15)));
    bearWarp.write("./Output/bearwarp.png");
    Image testWarp = warpBy1(test, segBefore, segAfter);
    testWarp.write("./Output/test_morph.png");

    // ------------------------------------------------

    // // Test Warp
    // Image werewolf("./Input/werewolf.png");
    // // ...
    //
    // // Test morph
    // // ... use the UI to specify segments
    // vector<Segment> segsBefore;
    // segsBefore.push_back(Segment(Vec2f(85, 102), Vec2f(119, 108)));
    // segsBefore.push_back(Segment(Vec2f(143, 117), Vec2f(169, 114)));
    // vector<Segment> segsAfter;
    // segsAfter.push_back(Segment(Vec2f(86, 93), Vec2f(114, 104)));
    // segsAfter.push_back(Segment(Vec2f(137, 94), Vec2f(152, 79)));
    //
    // // This is how you can write an image sequence
    // vector<Image> out = morph(fredo, werewolf,  segsBefore, segsAfter, 5);
    // int i = 0;
    // for (; i < out.size(); ++i) {
    //     ostringstream fname;
    //     fname << "./Output/test_morph_";
    //     fname << setfill('0') << setw(2);
    //     fname << i;
    //     fname << ".png";
    //     out[i].write(fname.str());
    // }
}
