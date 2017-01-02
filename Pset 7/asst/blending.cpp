#include "blending.h"
#include "matrix.h"
#include <ctime>

using namespace std;

Image blendingweight(int imwidth, int imheight) {
    // // --------- HANDOUT  PS07 ------------------------------
    Image newImage(imwidth, imheight, 1);
    for (int x = 0; x < imwidth; x++) {
      for (int y = 0; y < imheight; y++) {
        float newX = 1.0 - fabs(2.0 * (0.5f - (x / float(imwidth))));
        float newY = 1.0 - fabs(2.0 * (0.5f - (y / float(imheight))));
        newImage(x,y) = newX * newY;
      }
    }
    return newImage;
}

//  ****************************************************************************
//  * blending related functions re-written from previous asasignments
//  ****************************************************************************

// instead of writing source in out, *add* the source to out based on the weight
// so out(x,y) = out(x, y) + weight * image
void applyhomographyBlend(const Image &source, const Image &weight, Image &out, Matrix &H, bool bilinear) {
    // // --------- HANDOUT  PS07 ------------------------------
    Matrix HI = H.inverse();
    for (int x = 0; x < out.width(); x++) {
      for (int y = 0; y < out.height(); y++) {
        Vec3f newLoc = HI*Vec3f(x,y,1);
        float newX = newLoc[0] / float(newLoc[2]);
        float newY = newLoc[1] / float(newLoc[2]);
        for (int c = 0; c < out.channels(); c++) {
          if (newX < source.width() & newY < source.height() & newX >= 0 & newY >=0) {
            if (bilinear) {
              out(x,y,c) += interpolateLin(source, newX, newY, c, true) * interpolateLin(weight, newX, newY, 0, true);
            } else {
              out(x,y,c) += source(newX, newY, c) * weight(newX, newY, 0);
            }
          }
        }
      }
    }
    return;
}

Image stitchLinearBlending(const Image &im1, const Image &im2, const Image &we1, const Image &we2, Matrix H) {
    // // --------- HANDOUT  PS07 ------------------------------
    // stitch using image weights.
    // note there is no weight normalization.
    BoundingBox bbox1 = computeTransformedBBox(im1.width(), im1.height(), H);
    BoundingBox bbox2 = BoundingBox(0, im2.width(), 0, im2.height());
    BoundingBox bbox = bboxUnion(bbox1, bbox2);
    Matrix trans = makeTranslation(bbox);

    Image newImage(bbox.x2-bbox.x1, bbox.y2-bbox.y1, im1.channels());
    Matrix Htrans = trans*H;
    applyhomographyBlend(im2, we2, newImage, trans, true);
    applyhomographyBlend(im1, we1, newImage, Htrans, true);

    return newImage;
}

Image weightCompare(const Image &im1, const Image &im2, int width, int height, const Image &we1, const Image &we2, Matrix H) {
    // // --------- HANDOUT  PS07 ------------------------------
    // stitch using image weights.
    // note there is no weight normalization.
    BoundingBox bbox1 = computeTransformedBBox(we1.width(), we1.height(), H);
    BoundingBox bbox2 = BoundingBox(0, we2.width(), 0, we2.height());
    BoundingBox bbox = bboxUnion(bbox1, bbox2);
    Matrix trans = makeTranslation(bbox);

    Image newImage1(width, height, 1);
    Image newImage2(width, height, 1);
    Image im1Trans(width, height, 3);
    Image im2Trans(width, height, 3);
    Matrix Htrans = trans*H;
    applyHomography(we1, Htrans, newImage1, true);
    applyHomography(we2, trans, newImage2, true);
    applyHomography(im1, Htrans, im1Trans, true);
    applyHomography(im2, trans, im2Trans, true);
    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        if (newImage1(x,y) < newImage2(x,y)) {
          for (int c = 0; c < im1Trans.channels(); c++) {
            im1Trans(x,y,c) = im2Trans(x,y,c);
          }
        }
      }
    }
    return im1Trans;
}

Image normalMap(int width, int height, const Image &we1, const Image &we2, Matrix H) {
    Image newImage(width, height, 1);
    BoundingBox bbox1 = computeTransformedBBox(we1.width(), we1.height(), H);
    BoundingBox bbox2 = BoundingBox(0, we2.width(), 0, we2.height());
    BoundingBox bbox = bboxUnion(bbox1, bbox2);
    Matrix trans = makeTranslation(bbox);
    Matrix Htrans = trans*H;
    Image white1(we1.width(), we1.height(), 1);
    for (int i = 0; i < white1.number_of_elements(); i++) {
      white1(i) = 1.0f;
    }
    Image white2(we2.width(), we2.height(), 1);
    for (int i = 0; i < white2.number_of_elements(); i++) {
      white2(i) = 1.0f;
    }
    applyhomographyBlend(white1, we1, newImage, trans, true);
    applyhomographyBlend(white2, we2, newImage, Htrans, true);
    return newImage;
}

/*****************************************************************************
 * blending functions Pset 08
 *****************************************************************************/


// low freq and high freq (2-scale decomposition)
vector<Image> scaledecomp(const Image &im, float sigma) {
    vector <Image> ims;
    ims.push_back(gaussianBlur_separable(im, sigma));
    ims.push_back(im - ims[0]);
    return ims;
}

// stitch using different blending models
// blend can be 0 (none), 1 (linear) or 2 (2-layer)
Image stitchBlending(Image &im1, Image &im2, Matrix H, int blend) {
    // // --------- HANDOUT  PS07 ------------------------------
    Image we1 = blendingweight(im1.width(), im1.height());
    Image we2 = blendingweight(im2.width(), im2.height());

    if (blend == 0) {
      BoundingBox bbox1 = computeTransformedBBox(im1.width(), im1.height(), H);
      BoundingBox bbox2 = BoundingBox(0, im2.width(), 0, im2.height());
      BoundingBox bbox = bboxUnion(bbox1, bbox2);
      Matrix trans = makeTranslation(bbox);

      Image newImage(bbox.x2-bbox.x1, bbox.y2-bbox.y1, im1.channels());
      applyHomography(im2, trans, newImage, true);
      applyHomography(im1, trans*H, newImage, true);
      return newImage;
    } else if (blend == 1) {
      Image stitched = stitchLinearBlending(im1, im2, we1, we2, H);
      Image normMap = normalMap(stitched.width(), stitched.height(), we1, we2, H);
      for (int x = 0; x < stitched.width(); x++) {
        for (int y = 0; y < stitched.height(); y++) {
          for (int c = 0; c < stitched.channels(); c++) {
            if (normMap(x,y) != 0) {
              stitched(x,y,c) /= normMap(x,y);
            }
          }
        }
      }
      return stitched;
    } else if (blend == 2) {
      vector<Image> im1Decomp = scaledecomp(im1, 2.0f);
      vector<Image> im2Decomp = scaledecomp(im2, 2.0f);
      Image blurredStiched = stitchBlending(im1Decomp[0], im2Decomp[0], H, 1);
      Image focused = weightCompare(im1Decomp[1], im2Decomp[1], blurredStiched.width(),  blurredStiched.height(), we1, we2, H);
      focused.write("./Output/test.png");
      return focused + blurredStiched;
    }
    return Image(1,1,1);
}

// auto stitch
Image autostitch(Image &im1, Image &im2, int blend, float blurDescriptor, float radiusDescriptor) {
    // // --------- HANDOUT  PS07 ------------------------------
    vector<Point> im1Corners = HarrisCorners(im1);
    vector<Feature> im1Features = computeFeatures(im1, im1Corners, blurDescriptor, radiusDescriptor);
    vector<Point> im2Corners = HarrisCorners(im2);
    vector<Feature> im2Features = computeFeatures(im2, im2Corners, blurDescriptor, radiusDescriptor);

    vector<FeatureCorrespondence> corresp = findCorrespondences(im1Features, im2Features);
    Matrix H = RANSAC(corresp);
    return stitchBlending(im1, im2, H, blend);
}

/************************************************************************
 * Tricks: mini planets.
 ************************************************************************/

Image pano2planet(const Image &pano, int newImSize, bool clamp) {
    // // --------- HANDOUT  PS07 ------------------------------
    Image newImage(newImSize, newImSize, pano.channels());
    float center = newImSize / 2.0f;
    float angleMult = pano.width() / (2 * M_PI);
    float radiusMult = pano.height() / center;

    for (int x = 0; x < newImSize; x++) {
      for (int y = 0; y < newImSize; y++) {
        int refx = x - center;
        int refy = y - center;
        float theta = -atan2(refy, refx);
        float r = sqrt(pow(refx, 2.0) + pow(refy, 2.0));
        for (int c = 0; c < pano.channels(); c++) {
          if (theta > 0) {
            newImage(x,y,c) = interpolateLin(pano, angleMult * theta, pano.height() - radiusMult * r, c, true);
          } else {
            newImage(x,y,c) = interpolateLin(pano, pano.width() + angleMult * theta, pano.height() - radiusMult * r, c, true);
          }
        }
      }
    }
    return newImage;
}


/************************************************************************
 * 6.865: Stitch N images into a panorama
 ************************************************************************/

// Pset08-865. Compute sequence of N-1 homographies going from Im_i to Im_{i+1}
// Implement me!
vector<Matrix> sequenceHs(vector<Image> ims, float blurDescriptor, float radiusDescriptor) {
    // // --------- HANDOUT  PS07 ------------------------------

    vector<Matrix> Hs;
    for (int i = 0; i < ims.size() - 1; i++) {
      vector<Point> im1Corners = HarrisCorners(ims[i]);
      vector<Feature> im1Features = computeFeatures(ims[i], im1Corners, blurDescriptor, radiusDescriptor);
      vector<Point> im2Corners = HarrisCorners(ims[i+1]);
      vector<Feature> im2Features = computeFeatures(ims[i+1], im2Corners, blurDescriptor, radiusDescriptor);

      vector<FeatureCorrespondence> corresp = findCorrespondences(im1Features, im2Features);
      Matrix H = RANSAC(corresp);
      Hs.push_back(H);
    }
    return Hs;
}

// stack homographies:
//   transform a list of (N-1) homographies that go from I_i to I_i+1
//   to a list of N homographies going from I_i to I_refIndex.
vector <Matrix> stackHomographies(vector <Matrix> Hs, int refIndex) {
    // // --------- HANDOUT  PS07 ------------------------------
    vector<Matrix> result;
    for (int i = 0; i <= Hs.size(); i++) {
      Matrix newH(3,3);
      newH << 1, 0, 0,
              0, 1, 0,
              0, 0, 1;
      if (i < refIndex) {
        newH = Hs[i];
        for (int j = i + 1; j < refIndex; j++){
          newH =  Hs[j] * newH;
        }
      } else if (i > refIndex) {
        newH = Hs[i - 1].inverse();
        for (int j = i - 2; j >= refIndex; j--){
          newH = Hs[j].inverse() * newH;
        }
      }
      result.push_back(newH);
    }
    return result;
}


// Pset08-865: compute bbox around N images given one main reference.
BoundingBox bboxN(const vector<Matrix> &Hs, const vector<Image> &ims) {
    // // --------- HANDOUT  PS07 ------------------------------
    BoundingBox bbox(0,0,0,0);
    for (int i = 0; i < ims.size(); i++) {
      BoundingBox bbox1 = computeTransformedBBox(ims[i].width(), ims[i].height(), Hs[i]);
      bbox = bboxUnion(bbox1, bbox);
    }
    return bbox;
}

// Pset08-865.
// Implement me!
Image autostitchN(vector<Image> ims, int refIndex, float blurDescriptor, float radiusDescriptor) {
    // // --------- HANDOUT  PS07 ------------------------------
    vector<Matrix> Hs = sequenceHs(ims, blurDescriptor, radiusDescriptor);
    vector<Matrix> newHs = stackHomographies(Hs, refIndex);
    BoundingBox bbox = bboxN(newHs, ims);
    Matrix trans = makeTranslation(bbox);

    Image newImage(bbox.x2-bbox.x1, bbox.y2-bbox.y1, ims[0].channels());
    Image weights(bbox.x2-bbox.x1, bbox.y2-bbox.y1, 1);

    for (int i = 0; i < ims.size(); i++) {
      Image weight = blendingweight(ims[i].width(), ims[i].height());
      Image white(weight.width(), weight.height(), 1);
      for (int i = 0; i < white.number_of_elements(); i++) {
        white(i) = 1.0f;
      }
      Matrix transH = trans*newHs[i];
      applyhomographyBlend(ims[i], weight, newImage, transH, true);
      applyhomographyBlend(white, weight, weights, transH, true);
    }
    for (int x = 0; x < newImage.width(); x++) {
      for (int y = 0; y < newImage.height(); y++) {
        for (int c = 0; c < newImage.channels(); c++) {
          if (weights(x,y) != 0) {
            newImage(x,y,c) /= weights(x,y);
          }
        }
      }
    }
    return newImage;
}

Image autostitchCropN(vector<Image> ims, int refIndex, float blurDescriptor, float radiusDescriptor) {
    // // --------- HANDOUT  PS07 ------------------------------
    vector<Matrix> Hs = sequenceHs(ims, blurDescriptor, radiusDescriptor);
    vector<Matrix> newHs = stackHomographies(Hs, refIndex);
    BoundingBox bbox = bboxN(newHs, ims);
    Matrix trans = makeTranslation(bbox);

    Image newImage(bbox.x2-bbox.x1, bbox.y2-bbox.y1, ims[0].channels());
    Image weights(bbox.x2-bbox.x1, bbox.y2-bbox.y1, 1);

    for (int i = 0; i < ims.size(); i++) {
      Image weight = blendingweight(ims[i].width(), ims[i].height());
      Image white(weight.width(), weight.height(), 1);
      for (int i = 0; i < white.number_of_elements(); i++) {
        white(i) = 1.0f;
      }
      Matrix transH = trans*newHs[i];
      applyhomographyBlend(ims[i], weight, newImage, transH, true);
      applyhomographyBlend(white, weight, weights, transH, true);
    }
    for (int x = 0; x < newImage.width(); x++) {
      for (int y = 0; y < newImage.height(); y++) {
        for (int c = 0; c < newImage.channels(); c++) {
          if (weights(x,y) != 0) {
            newImage(x,y,c) /= weights(x,y);
          }
        }
      }
    }
    int cropCount = 1;
    int cx1 = 0;
    int cx2 = weights.width() - 1;
    int cy1 = 0;
    int cy2 = weights.height() - 1;
    while (cropCount != 0) {
      int x1Diff = 0;
      int x2Diff = 0;
      int y1Diff = 0;
      int y2Diff = 0;
      for (int x = cx1; x <= cx2; x++) {
        if (weights(x,cy1) == 0) {
          y1Diff += 1;
        }
        if (weights(x,cy2) == 0) {
          y2Diff += 1;
        }
      }
      for (int y = cy1; y <= cy2; y++) {
        if (weights(cx1,y) == 0) {
          x1Diff += 1;
        }
        if (weights(cx2,y) == 0) {
          x2Diff += 1;
        }
      }
      if (x1Diff > x2Diff && x1Diff > y1Diff && x1Diff > y2Diff) {
        cx1 +=1;
      } else {
        if (x2Diff > y1Diff && x2Diff > y2Diff) {
          cx2 -= 1;
        } else {
          if (y1Diff > y2Diff) {
            cy1 += 1;
          } else {
            cy2 -= 1;
          }
        }
      }
      cropCount = x1Diff + x2Diff + y1Diff + y2Diff;
    }
    Image croppedImage(cx2-cx1, cy2-cy1,newImage.channels());
    for (int x = 0; x < croppedImage.width(); x++) {
      for (int y = 0; y < croppedImage.height(); y++) {
        for (int c = 0; c < croppedImage.channels(); c++) {
          croppedImage(x,y,c) = newImage(x+cx1, y+cy1, c);
        }
      }
    }
    return croppedImage;
}

/******************************************************************************
 * Helper functions
 *****************************************************************************/

// copy a single-channeled image to several channels
Image copychannels(const Image &im, int nChannels) {
    // image must have one channel
    assert(im.channels() == 1);
    Image oim(im.width(), im.height(), nChannels);

    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            for (int c = 0; c < nChannels; c++) {
                oim(i, j, c) = im(i, j);
            }
        }
    }
    return oim;
}
//
