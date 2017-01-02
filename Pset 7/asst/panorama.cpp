#include "panorama.h"
#include "matrix.h"
#include <unistd.h>
#include <ctime>

using namespace std;

Image computeTensor(const Image &im, float sigmaG, float factorSigma) {
    // // --------- HANDOUT  PS07 ------------------------------
    // Compute xx/xy/yy Tensor of an image. (stored in that order)

    Image newImage(im.width(), im.height(), 3);
    Image lum = lumiChromi(im)[0];
    lum = gaussianBlur_separable(lum, sigmaG);
    Image gradX = gradientX(lum);
    Image gradY = gradientY(lum);
    for (int x = 0; x < im.width(); x++) {
      for (int y = 0; y < im.height(); y++) {
        newImage(x,y,0) = float(pow(gradX(x,y), 2.0f));
        newImage(x,y,1) = float(gradX(x,y) * gradY(x,y));
        newImage(x,y,2) = float(pow(gradY(x,y), 2.0f));
      }
    }
    return gaussianBlur_separable(newImage, sigmaG * factorSigma);
}

Image cornerResponse(const Image &im, float k, float sigmaG,
        float factorSigma)
{
    // // --------- HANDOUT  PS07 ------------------------------
    // Compute response = det(M) - k*[(trace(M)^2)] at every pixel location,
    // using the structure tensor of im.
    Image newImage(im.width(), im.height(), 1);
    Image M = computeTensor(im, sigmaG, factorSigma);

    for (int x = 0; x < im.width(); x++) {
      for (int y = 0; y < im.height(); y++) {
        newImage(x,y) = M(x,y,0)*M(x,y,2) - pow(M(x,y,1), 2.0) - k * pow(M(x,y,0) + M(x,y,2),2.0);
      }
    }
    return newImage;
  }


vector<Point> HarrisCorners(const Image &im, float k, float sigmaG,
        float factorSigma, float maxiDiam, float boundarySize)
{
    // // --------- HANDOUT  PS07 ------------------------------
    // Compute Harris Corners by maximum filtering the cornerResponse map.
    // The corners are the local maxima.
    Image corners = cornerResponse(im, k, sigmaG, factorSigma);
    Image filteredCorners = maximum_filter(corners, maxiDiam);
    corners = corners - filteredCorners;
    vector<Point> points;
    for (int x = boundarySize; x < im.width() - boundarySize; x++) {
      for (int y = boundarySize; y < im.height() - boundarySize; y++) {
        if (corners(x,y) == 0) {
          points.push_back(Point(x,y));
        }
      }
    }

    return points;
}


Image descriptor(const Image &blurredIm, Point p, float radiusDescriptor) {
    // // --------- HANDOUT  PS07 ------------------------------
    // Extract a descriptor from blurredIm around point p, with a radius 'radiusDescriptor'.
    Image newImage(radiusDescriptor * 2 + 1, radiusDescriptor * 2 + 1 , 1);
    for (int x = 0; x < radiusDescriptor * 2 + 1; x++) {
      for (int y = 0; y < radiusDescriptor * 2 + 1; y++) {
        newImage(x,y) = blurredIm(p.x + x - radiusDescriptor, p.y + y - radiusDescriptor);
      }
    }
    float mean = newImage.mean();
    newImage = newImage - mean;
    float stdev = sqrt(newImage.var());
    return newImage / stdev;
}


vector <Feature> computeFeatures(const Image &im, vector<Point> cornersL,
    float sigmaBlurDescriptor, float radiusDescriptor) {
    // // --------- HANDOUT  PS07 ------------------------------
    // Pset07. obtain corner features from a list of corner points

    Image blurredIm = gaussianBlur_separable(lumiChromi(im)[0], sigmaBlurDescriptor);
    vector<Feature> features;
    for (int i = 0; i < cornersL.size(); i++) {
      features.push_back(Feature(cornersL[i], descriptor(blurredIm, cornersL[i], radiusDescriptor)));
    }
    return features;
}



float l2Features(Feature &f1, Feature &f2) {
    // // --------- HANDOUT  PS07 ------------------------------
    // Compute the squared Euclidean distance between the descriptors of f1, f2.
    Image f1Img = f1.desc();
    Image f2Img = f2.desc();
    f1Img = f1Img - f2Img;
    float result = 0.0f;
    for (int i = 0; i < f1Img.number_of_elements(); i++) {
      result += pow(f1Img(i), 2.0f);
    }
    return result;
}


vector <FeatureCorrespondence> findCorrespondences(vector<Feature> listFeatures1, vector<Feature> listFeatures2, float threshold) {
    // // --------- HANDOUT  PS07 ------------------------------
    // Find correspondences between listFeatures1 and listFeatures2 using the
    // second-best test.

    vector<FeatureCorrespondence> corres;
    threshold = pow(threshold, 2.0f);

    for (int i = 0; i < listFeatures1.size(); i++) {
      float bestDist = INFINITY;
      float secondDist = INFINITY;
      int bestFeature = -1;
      for (int j = 0; j < listFeatures2.size(); j++) {
        float newDist = l2Features(listFeatures1[i], listFeatures2[j]);
        if (newDist < bestDist) {
          secondDist = bestDist;
          bestDist = newDist;
          bestFeature = j;
        } else if (newDist < secondDist) {
          secondDist = newDist;
        }
      }
      if ((secondDist / bestDist) >= threshold) {
        corres.push_back(FeatureCorrespondence(listFeatures1[i], listFeatures2[bestFeature]));
      }
    }
    return corres;
}


vector<bool> inliers(Matrix H, vector <FeatureCorrespondence> listOfCorrespondences, float epsilon) {
    // // --------- HANDOUT  PS07 ------------------------------
    // Pset07: Implement as part of RANSAC
    // return a vector of bools the same size as listOfCorrespondences indicating
    //  whether each correspondance is an inlier according to the homography H and threshold epsilon

    vector<bool> results;
    for (int i = 0; i < listOfCorrespondences.size(); i++) {
      Vec3f p1 = listOfCorrespondences[i].feature(1).point().toHomogenousCoords();
      p1 = H.inverse() * p1;
      p1 = p1 / p1[2];
      p1 = p1 - listOfCorrespondences[i].feature(0).point().toHomogenousCoords();
      results.push_back(p1.norm() < epsilon);
    }
    return results;
}

Matrix RANSAC(vector <FeatureCorrespondence> listOfCorrespondences, int Niter, float epsilon) {
    // // --------- HANDOUT  PS07 ------------------------------
    // Put together the RANSAC algorithm.

    int bestSum = 0;
    Matrix bestH(3,3);

    for (int n = 0; n < Niter; n++) {
      vector<CorrespondencePair> pairs = getListOfPairs(sampleFeatureCorrespondences(listOfCorrespondences));
      Matrix H = computeHomography(pairs.data());
      if (H.determinant() != 0) {
        vector<bool> checks = inliers(H, listOfCorrespondences, epsilon);
        int newSum = 0;
        for (int i = 0; i < checks.size(); i++) {
          if (checks[i]) {
            newSum++;
          }
        }
        if (newSum > bestSum) {
          bestSum = newSum;
          bestH = H;
        }
      }
    }
    return bestH;
}


Image autostitch(Image &im1, Image &im2, float blurDescriptor, float radiusDescriptor) {
    // // --------- HANDOUT  PS07 ------------------------------
    // Now you have all the ingredients to make great panoramas without using a
    // primitive javascript UI !
    vector<Point> im1Corners = HarrisCorners(im1);
    vector<Feature> im1Features = computeFeatures(im1, im1Corners, blurDescriptor, radiusDescriptor);
    vector<Point> im2Corners = HarrisCorners(im2);
    vector<Feature> im2Features = computeFeatures(im2, im2Corners, blurDescriptor, radiusDescriptor);

    vector<FeatureCorrespondence> corresp = findCorrespondences(im1Features, im2Features);
    Matrix H = RANSAC(corresp);
    BoundingBox bbox1 = computeTransformedBBox(im1.width(), im1.height(), H);
    BoundingBox bbox2 = BoundingBox(0, im2.width(), 0, im2.height());
    BoundingBox bbox = bboxUnion(bbox1, bbox2);
    Matrix trans = makeTranslation(bbox);

    Image newImage(bbox.x2-bbox.x1, bbox.y2-bbox.y1, im1.channels());
    applyHomography(im2, trans, newImage, true);
    applyHomography(im1, trans*H, newImage, true);

    return newImage;
}




// *****************************************************************************
//  * Helpful optional functions to implement
// ****************************************************************************

Image getBlurredLumi(const Image &im, float sigmaG) {
    return Image(1,1,1);
}

int countBoolVec(vector<bool> ins) {
    return 0;
}

// *****************************************************************************
//  * Do Not Modify Below This Point
// *****************************************************************************

// Pset07 RANsac helper. re-shuffle a list of correspondances
vector<FeatureCorrespondence> sampleFeatureCorrespondences(vector <FeatureCorrespondence> listOfCorrespondences) {
    random_shuffle(listOfCorrespondences.begin(), listOfCorrespondences.end());
    return listOfCorrespondences;
}

// Pset07 RANsac helper: go from 4 correspondances to a list of points [4][2][3] as used in Pset06.
// Note: The function uses the first 4 correspondances passed
vector<CorrespondencePair> getListOfPairs(vector <FeatureCorrespondence> listOfCorrespondences) {
    vector<CorrespondencePair> out;
    for (int i = 0; i < 4; i++) {
        out.push_back(listOfCorrespondences[i].toCorrespondencePair());
    }
    return out;
}

// Corner visualization.
Image visualizeCorners(const Image &im, vector<Point> pts,
        int rad, const vector <float> & color)
{
    Image vim = im;
    for (int i = 0; i < (int) pts.size(); i++) {
        int px = pts[i].x;
        int py = pts[i].y;

        int minx = max(px - rad, 0);

        for (int delx = minx; delx < min(im.width(), px + rad + 1); delx++)
        for (int dely = max(py - rad, 0); dely < min(im.height(), py + rad + 1); dely++)
        {
            if ( sqrt(pow(delx-px, 2) + pow(dely - py, 2)) <= rad) {
                for (int c = 0; c < im.channels(); c++) {
                    vim(delx, dely, c) = color[c];
                }
            }
        }
    }
    return vim;
}

Image visualizeFeatures(const Image &im, vector <Feature> LF, float radiusDescriptor) {
    // assumes desc are within image range
    Image vim = im;
    int rad = radiusDescriptor;

    for (int i = 0; i < (int) LF.size(); i++) {
        int px = LF[i].point().x;
        int py = LF[i].point().y;
        Image desc = LF[i].desc();

        for (int delx = px - rad; delx < px + rad + 1; delx++) {
            for (int dely = py - rad; dely < py + rad + 1; dely++) {
                vim(delx, dely, 0) = 0;
                vim(delx, dely, 1) = 0;
                vim(delx, dely, 2) = 0;

                if (desc(delx - (px-rad), dely - (py - rad)) > 0) {
                    vim(delx, dely, 1) = 1;
                } else if (desc(delx - (px-rad), dely - (py - rad)) < 0) {
                    vim(delx, dely, 0) = 1;
                }
            }
        }
    }
    return vim;
}

void drawLine(Point p1, Point p2, Image &im,  const vector<float> & color) {
    float minx = min(p1.x, p2.x);
    float miny = min(p1.y, p2.y);
    float maxx = max(p1.x, p2.x);
    float maxy = max(p1.y, p2.y);

    int spaces = 1000;
    for (int i = 0; i < spaces; i++) {
        float x = minx + (maxx - minx) / spaces * (i+1);
        float y = miny + (maxy - miny) / spaces * (i+1);
        for (int c = 0; c < im.channels(); c++) {
            im(x, y, c) = color[c];
        }
    }
}

Image visualizePairs(const Image &im1, const Image &im2, vector<FeatureCorrespondence> corr) {
    Image vim(im1.width() + im2.width(), im1.height(), im1.channels());

    // stack the images
    for (int j = 0; j < im1.height(); j++) {
        for (int c = 0; c < im1.channels(); c++) {
            for (int i = 0; i < im1.width(); i++) {
                vim(i,j,c) = im1(i,j,c);
            }
            for (int i = 0; i < im2.width(); i++) {
                vim(i+im1.width(),j,c) = im2(i,j,c);
            }
        }
    }

    // draw lines
    for (int i = 0; i < (int) corr.size(); i++) {
        Point p1 = corr[i].feature(0).point();
        Point p2 = corr[i].feature(1).point();
        p2.x = p2.x + im1.width();
        drawLine(p1, p2, vim);
    }
    return vim;
}

Image visualizePairsWithInliers(const Image &im1, const Image &im2, vector<FeatureCorrespondence> corr, const vector<bool> & ins) {
    Image vim(im1.width() + im2.width(), im1.height(), im1.channels());

    // stack the images
    for (int j = 0; j < im1.height(); j++) {
        for (int c = 0; c < im1.channels(); c++) {
            for (int i = 0; i < im1.width(); i++) {
                vim(i,j,c) = im1(i,j,c);
            }
            for (int i = 0; i < im2.width(); i++) {
                vim(i+im1.width(),j,c) = im2(i,j,c);
            }
        }
    }

    // draw lines
    vector<float> red(3,0);
    vector<float> green(3,0);
    red[0] = 1.0f;
    green[1]= 1.0f;

    for (int i = 0; i < (int) corr.size(); i++) {
        Point p1 = corr[i].feature(0).point();
        Point p2 = corr[i].feature(1).point();
        p2.x = p2.x + im1.width();
        if (ins[i]) {
            drawLine(p1, p2, vim, green);
        } else {
            drawLine(p1, p2, vim, red);
        }
    }
    return vim;

}

// Inliers:  Detected corners are in green, reprojected ones are in red
// Outliers: Detected corners are in yellow, reprojected ones are in blue
vector<Image> visualizeReprojection(const Image &im1, const Image &im2, Matrix  H, vector<FeatureCorrespondence> & corr, const vector<bool> & ins) {
    // Initialize colors
    vector<float> red(3,0);
    vector<float> green(3,0);
    vector<float> blue(3,0);
    vector<float> yellow(3,0);
    red[0] = 1.0f;
    green[1]= 1.0f;
    blue[2] = 1.0f;
    yellow[0] = 1.0f;
    yellow[1] = 1.0f;

    vector<Point> detectedPts1In;
    vector<Point> projectedPts1In;
    vector<Point> detectedPts1Out;
    vector<Point> projectedPts1Out;

    vector<Point> detectedPts2In;
    vector<Point> projectedPts2In;
    vector<Point> detectedPts2Out;
    vector<Point> projectedPts2Out;

    for (int i = 0 ; i < (int) corr.size(); i++) {
        Point pt1 = corr[i].feature(0).point();
        Point pt2 = corr[i].feature(1).point();
        Matrix P1 = pt1.toHomogenousCoords();
        Matrix P2 = pt2.toHomogenousCoords();
        Matrix P2_proj = H*P1;
        Matrix P1_proj = H.inverse()*P2;
        Point reproj1 = Point(P1_proj(0)/P1_proj(2), P1_proj(1)/P1_proj(2));
        Point reproj2 = Point(P2_proj(0)/P2_proj(2), P2_proj(1)/P2_proj(2));
        if (ins[i]) { // Inlier
            detectedPts1In.push_back(pt1);
            projectedPts1In.push_back(reproj1);
            detectedPts2In.push_back(pt2);
            projectedPts2In.push_back(reproj2);
        } else { // Outlier
            detectedPts1Out.push_back(pt1);
            projectedPts1Out.push_back(reproj1);
            detectedPts2Out.push_back(pt2);
            projectedPts2Out.push_back(reproj2);
        }
    }

    vector<Image> output;
    Image vim1(im1);
    Image vim2(im2);
    vim1 = visualizeCorners(im1, detectedPts1In,2, green);
    vim1 = visualizeCorners(vim1, projectedPts1In,1, red);
    vim1 = visualizeCorners(vim1, detectedPts1Out,2, yellow);
    vim1 = visualizeCorners(vim1, projectedPts1Out,1, blue);

    vim2 = visualizeCorners(im2, detectedPts2In,2, green);
    vim2 = visualizeCorners(vim2, projectedPts2In,1, red);
    vim2 = visualizeCorners(vim2, detectedPts2Out,2, yellow);
    vim2 = visualizeCorners(vim2, projectedPts2Out,1, blue);


    output.push_back(vim1);
    output.push_back(vim2);
    return output;
}



/***********************************************************************
 * Point and Feature Definitions *
 **********************************************************************/
Point::Point(int xp, int yp) : x(xp), y(yp) {}
Point::Point() : x(0.0f), y(0.0f) {}
void Point::print() { printf("(%d, %d)\n", x, y); }
Vec3f Point::toHomogenousCoords() {
    Vec3f b;
    b(0) = x;
    b(1) = y;
    b(2) = 1;
    return b;
}

// Feature Constructors
Feature::Feature(Point ptp, const Image &descp)
    : pt(ptp), dsc(descp)
{
    pt = ptp;
    dsc = descp;
}

// getter functions
Point Feature::point() { return pt;}
Image Feature::desc() { return dsc;}

// printer
void Feature::print() {
    printf("Feature:");
    point().print();
    for (int j = 0; j < dsc.height(); j++) {
        for (int i = 0; i < dsc.width(); i++) {
            printf("%+07.2f ", dsc(i, j));
        }
        printf("\n");
    }
}

// FeatureCorrespondence Constructors
FeatureCorrespondence::FeatureCorrespondence(const Feature &f1p, const Feature &f2p)
    : f1(f1p), f2(f2p)
{
}


vector<Feature> FeatureCorrespondence::features() {
    vector<Feature> feats;
    feats.push_back(f1);
    feats.push_back(f2);
    return feats;
}


Feature FeatureCorrespondence::feature(int i) {
    if (i == 0)
        return f1;
    else
        return f2;
}


// printer
void FeatureCorrespondence::print() {
    printf("FeatureCorrespondence:");
    f1.print();
    f2.print();
}


CorrespondencePair FeatureCorrespondence::toCorrespondencePair() {

    return CorrespondencePair(
        (float) f1.point().x,
        (float) f1.point().y,
        1,
        (float) f2.point().x,
        (float) f2.point().y,
        1
    );
}
