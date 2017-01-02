/* -----------------------------------------------------------------
 * File:    morphing.cpp
 * Created: 2015-09-25
 * -----------------------------------------------------------------
 *
 *
 *
 * ---------------------------------------------------------------*/




#include <cassert>
#include "morphing.h"

using namespace std;

Vec2f add(const Vec2f & a, const Vec2f & b) {
    // --------- HANDOUT  PS03 ------------------------------
    // Return the vector sum of a an b
    return Vec2f(a.x + b.x, a.y + b.y); // change me

}


Vec2f subtract(const Vec2f & a, const Vec2f & b) {
    // --------- HANDOUT  PS03 ------------------------------
    // Return a-b
    return Vec2f(a.x - b.x, a.y - b.y); // change me
}


Vec2f scalarMult(const Vec2f & a, float f) {
    // --------- HANDOUT  PS03 ------------------------------
    // Return a*f
    return Vec2f(a.x * f, a.y * f); // change me
}


float dot(const Vec2f & a, const Vec2f & b) {
    // --------- HANDOUT  PS03 ------------------------------
    // Return the dot product of a and b.
    return (a.x * b.x) + (a.y * b.y); // change me
}

float length(const Vec2f & a) {
    // --------- HANDOUT  PS03 ------------------------------
    // Return the length of a.
    return sqrt(pow(a.x,2) + pow(a.y, 2)); // change me
}


// The Segment constructor takes in 2 points P(x1,y1) and Q(x2,y2) correspoding to
// the ends of a segment and initialize the local reference frame e1,e2.
Segment::Segment(Vec2f P_, Vec2f Q_) : P(P_), Q(Q_) {
    // // --------- HANDOUT  PS03 ------------------------------
    // // The initializer list above ": P(P_), Q(Q_)" already copies P_
    // // and Q_, so you don't have to do it in the body of the constructor.
    // You should:
    // * Initialize the local frame e1,e2 (see header file)
    Vec2f base = subtract(Q,P);
    float baseLen = length(base);
    e1 = scalarMult(base, 1/baseLen);
    e2 = scalarMult(Vec2f(base.y, base.x), 1/(baseLen));
}


Vec2f Segment::XtoUV(Vec2f X) const {
    // --------- HANDOUT  PS03 ------------------------------
    // Compute the u,v coordinates of a point X with
    // respect to the local frame of the segment.
    // e2 ^
    //    |
    // v  +  * X
    //    | /
    //    |/
    //    *--+------>-----*
    //    P  u     e1     Q
    //                    u=1
    //
    // * Be careful with the different normalization for u and v
    float u = dot(subtract(X,P), getE1()) / length(subtract(Q,P));
    float v = dot(subtract(X,P), getE2());
    return Vec2f(u, v);
}


Vec2f Segment::UVtoX(Vec2f uv) const {
    // --------- HANDOUT  PS03 ------------------------------
    // compute the (x, y) position of a point given by the (u,v)
    // location relative to this segment.
    // * Be careful with the different normalization for u and v
    Vec2f base = subtract(Q,P);
    return add(P, add(scalarMult(base,uv.x), (scalarMult(Vec2f(base.y, base.x), uv.y/length(base)))));
}


float Segment::distance(Vec2f X) const {
    // // --------- HANDOUT  PS03 ------------------------------
    // // Implement distance from a point X(x,y) to the segment. Remember the 3
    // // cases from class.
    return 0.0f;

}


Image warpBy1(const Image &im, const Segment &segBefore, const Segment &segAfter){
    // --------- HANDOUT  PS03 ------------------------------
    // Warp an entire image according to a pair of segments.
    Image newImage(im.width(), im.height(), im.channels());
    for (int y = 0; y < im.height(); y++) {
      for (int x = 0; x < im.width(); x++) {
        Vec2f uv = segAfter.XtoUV(Vec2f(x,y));
        Vec2f after = segBefore.UVtoX(uv);
        for (int c = 0; c < im.channels(); c++) {
          newImage(x,y,c) = im.smartAccessor(after.x, after.y, c, true);
        }
      }
    }
    return newImage;
}


float Segment::weight(Vec2f X, float a, float b, float p) const {
    // --------- HANDOUT  PS03 ------------------------------
    // compute the weight of a segment to a point X(x,y) given the weight
    // parameters a,b, and p (see paper for details).
    return 1.0f; // changeme
}


Image warp(const Image &im, const vector<Segment> &src_segs,
        const vector<Segment> &dst_segs, float a, float b, float p)
{
    // --------- HANDOUT  PS03 ------------------------------
    // Warp an image according to a vector of before and after segments using
    // segment weighting
    return im;
}


vector<Image> morph(const Image &im_before, const Image &im_after,
        const vector<Segment> &segs_before, const vector<Segment> &segs_after,
        int N, float a, float b, float p)
{
    // --------- HANDOUT  PS03 ------------------------------
    // return a vector of N+2 images: the two inputs plus N images that morphs
    // between im_before and im_after for the corresponding segments. im_before should be the first image, im_after the last.
    return vector<Image>();

}
