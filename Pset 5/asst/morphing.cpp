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
    return Vec2f(a.x + b.x, a.y + b.y);
}


Vec2f subtract(const Vec2f & a, const Vec2f & b) {
    // --------- HANDOUT  PS03 ------------------------------
    // Return a-b
    return Vec2f(a.x - b.x, a.y - b.y);
}


Vec2f scalarMult(const Vec2f & a, float f) {
    // --------- HANDOUT  PS03 ------------------------------
    // Return a*f
    return Vec2f(a.x * f, a.y * f);
}


float dot(const Vec2f & a, const Vec2f & b) {
    // --------- HANDOUT  PS03 ------------------------------
    // Return the dot product of a and b.
    return (a.x * b.x) + (a.y * b.y);
}

float length(const Vec2f & a) {
    // --------- HANDOUT  PS03 ------------------------------
    // Return the length of a.
    return sqrt(pow(a.x,2) + pow(a.y, 2));
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
    e2 = scalarMult(Vec2f(-base.y, base.x), 1/(baseLen));
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
    // std::cout << "(" << getE1().x << "," << getE1().y << ")" << std::endl;
    // std::cout << "(" << getE2().x << "," << getE2().y << ")" << std::endl;
    return Vec2f(u, v);
}


Vec2f Segment::UVtoX(Vec2f uv) const {
    // --------- HANDOUT  PS03 ------------------------------
    // compute the (x, y) position of a point given by the (u,v)
    // location relative to this segment.
    // * Be careful with the different normalization for u and v
    Vec2f base = subtract(Q,P);
    return add(P, add(scalarMult(base,uv.x), (scalarMult(getE2(), uv.y))));
}


float Segment::distance(Vec2f X) const {
    // // --------- HANDOUT  PS03 ------------------------------
    // // Implement distance from a point X(x,y) to the segment. Remember the 3
    // // cases from class.

    Vec2f xp = subtract(X,P);
    Vec2f pq = subtract(Q,P);
    float dotProd = dot(xp,pq) / pow(length(pq), 2.0);
    if (dotProd < 0 || dotProd > 1) {
      if (length(subtract(X,P)) < length(subtract(X,Q))) {
        return length(subtract(X,P));
      } else {
        return length(subtract(X,Q));
      }
    }
    Vec2f proj = scalarMult(pq, dotProd);
    Vec2f norm = subtract(xp, proj);
    // std::cout << "x: " << X.x << ", " << X.y << "    ||     proj: " << proj.x << ", " << proj.y  << "     ||     norm x: (" << norm.x << ", " << norm.y << ")" <<std::endl;
    // std::cout << "xp: " << xp.x << ", " << xp.y << "    ||     pq: " << pq.x << ", " << pq.y  <<std::endl;
    float dist = length(norm);
    // std::cout << dist << std::endl;
    return dist;
}


Image warpBy1(const Image &im, const Segment &segBefore, const Segment &segAfter){
    // --------- HANDOUT  PS03 ------------------------------
    // Warp an entire image according to a pair of segments.
    Image newImage(im.width(), im.height(), im.channels());
    for (int y = 0; y < im.height(); y++) {
      for (int x = 0; x < im.width(); x++) {
        Vec2f uv = segAfter.XtoUV(Vec2f(x,y));
        Vec2f after = segBefore.UVtoX(uv);
        // std::cout << "x: " << x << " y: " << y << "      ||      u: " << uv.x << " v: " << uv.y  << "        ||     X: " << after.x << " Y: " << after.y <<std::endl;
        for (int c = 0; c < im.channels(); c++) {
          newImage(x,y,c) = interpolateLin(im, after.x, after.y, c, true);
        }
      }
    }
    return newImage;
}


float Segment::weight(Vec2f X, float a, float b, float p) const {
    // --------- HANDOUT  PS03 ------------------------------
    // compute the weight of a segment to a point X(x,y) given the weight
    // parameters a,b, and p (see paper for details).
    return pow((pow(length(X),p) / (a + distance(X))),b);
}


Image warp(const Image &im, const vector<Segment> &src_segs,
        const vector<Segment> &dst_segs, float a, float b, float p)
{
    // --------- HANDOUT  PS03 ------------------------------
    // Warp an image according to a vector of before and after segments using
    // segment weighting
    Image newImage(im.width(), im.height(), im.channels());
    Image weights(im.width(), im.height(), im.channels());
    Image afterTrans(im.width(), im.height(), 2);
    for (int s = 0; s < src_segs.size(); s++) {
      for (int y = 0; y < im.height(); y++) {
        for (int x = 0; x < im.width(); x++) {
          Vec2f uv = src_segs[s].XtoUV(Vec2f(x,y));
          Vec2f after = dst_segs[s].UVtoX(uv);
          float weight = dst_segs[s].weight(Vec2f(x,y), a, b, p);
          afterTrans(x,y,0) += (x - after.x) * weight;
          afterTrans(x,y,1) += (y - after.y) * weight;
          weights(x,y) += weight;
          // std::cout << "x: " << x << " y: " << y << "      ||      u: " << uv.x << " v: " << uv.y  << "        ||     X: " << after.x << " Y: " << after.y <<std::endl;
        }
      }
    }
    for (int y = 0; y < im.height(); y++) {
      for (int x = 0; x < im.width(); x++) {
        for (int c = 0; c < im.channels(); c++) {
          float xCord = x + afterTrans(x,y,0) / weights(x,y);
          float yCord = y + afterTrans(x,y,1) / weights(x,y);
          newImage(x,y,c) = interpolateLin(im, xCord, yCord, c, true);
        }
      }
    }
    return newImage;
}


vector<Image> morph(const Image &im_before, const Image &im_after,
        const vector<Segment> &segs_before, const vector<Segment> &segs_after,
        int N, float a, float b, float p)
{
    // --------- HANDOUT  PS03 ------------------------------
    // return a vector of N+2 images: the two inputs plus N images that morphs
    // between im_before and im_after for the corresponding segments. im_before should be the first image, im_after the last.
    vector<Image> results;
    results.push_back(im_before);

    for (int n = 1; n <= N; n++) {
      Image interImg(im_before.width(), im_before.height(), im_before.channels());
      float t = n / float(N+1);
      vector<Segment> targetSegs;
      for (int i = 0; i < segs_before.size(); i++) {
        Vec2f P = add(scalarMult(subtract(segs_after[i].getP(), segs_before[i].getP()), t), segs_before[i].getP());
        Vec2f Q = add(scalarMult(subtract(segs_after[i].getQ(), segs_before[i].getQ()), t), segs_before[i].getQ());
        targetSegs.push_back(Segment(P,Q));
      }
      Image before = warp(im_before, segs_before, targetSegs, a, b, p);
      Image after = warp(im_after, segs_after, targetSegs, a, b, p);
      for (int j = 0; j < before.number_of_elements(); j++) {
        interImg(j) = before(j) * (1 - t) + after(j) * t;
      }
      results.push_back(interImg);
    }
    results.push_back(im_after);
    return results;
}
