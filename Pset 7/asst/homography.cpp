#include "homography.h"
#include "matrix.h"

using namespace std;


void applyHomography(const Image &source, const Matrix &H, Image &out, bool bilinear) {
    // // --------- HANDOUT  PS06 ------------------------------
    // Transform image source using the homography H, and composite in onto out.
    // if bilinear == true, using bilinear interpolation. Use nearest neighbor
    // otherwise.
    Matrix HI = H.inverse();
    for (int x = 0; x < out.width(); x++) {
      for (int y = 0; y < out.height(); y++) {
        Vec3f newLoc = HI*Vec3f(x,y,1);
        float newX = newLoc[0] / float(newLoc[2]);
        float newY = newLoc[1] / float(newLoc[2]);
        for (int c = 0; c < out.channels(); c++) {
          if (newX < source.width() & newY < source.height() & newX >= 0 & newY >=0) {
            if (bilinear) {
              out(x,y,c) = interpolateLin(source, newX, newY, c, true);
            } else {
              out(x,y,c) = source(newX, newY, c);
            }
          }
        }
      }
    }
    return;
}

Matrix computeHomography(const CorrespondencePair correspondences[4]) {
    // --------- HANDOUT  PS06 ------------------------------
    // Compute a homography from 4 point correspondences.
    Matrix A = Matrix::Zero(8,8);
    Matrix B = Matrix::Zero(8,1);

    for (int i = 0; i < 4; i++) {
      Vec3f start = correspondences[i].point1;
      Vec3f finish = correspondences[i].point2;
      int yRow = 2 * i;
      int xRow = 2 * i + 1;
      A(yRow,0) = start[0];
      A(yRow,1) = start[1];
      A(yRow,2) = 1;
      A(yRow,3) = 0;
      A(yRow,4) = 0;
      A(yRow,5) = 0;
      A(yRow,6) = -start[0] * finish[0];
      A(yRow,7) = -start[1] * finish[0];
      A(xRow,0) = 0;
      A(xRow,1) = 0;
      A(xRow,2) = 0;
      A(xRow,3) = start[0];
      A(xRow,4) = start[1];
      A(xRow,5) = 1;
      A(xRow,6) = -start[0] * finish[1];
      A(xRow,7) = -start[1] * finish[1];

      B(yRow,0) = finish[0];
      B(xRow,0) = finish[1];
    }

    Matrix x = A.fullPivLu().solve(B);
    Matrix H = Matrix::Zero(3,3);

    H(0,0) = x(0,0);
    H(0,1) = x(1,0);
    H(0,2) = x(2,0);
    H(1,0) = x(3,0);
    H(1,1) = x(4,0);
    H(1,2) = x(5,0);
    H(2,0) = x(6,0);
    H(2,1) = x(7,0);
    H(2,2) = 1;

    return H;
}


BoundingBox computeTransformedBBox(int imwidth, int imheight, Matrix H) {
    // --------- HANDOUT  PS06 ------------------------------
    // Predict the bounding boxes that encompasses all the transformed
    // coordinates for pixels frow and Image with size (imwidth, imheight
    Vec3f x0y0 = H*Vec3f(0, 0, 1);
    Vec3f x1y0 = H*Vec3f(imwidth, 0, 1);
    Vec3f x0y1 = H*Vec3f(0,imheight, 1);
    Vec3f x1y1 = H*Vec3f(imwidth, imheight, 1);

    x0y0 = x0y0 / x0y0[2];
    x1y0 = x1y0 / x1y0[2];
    x0y1 = x0y1 / x0y1[2];
    x1y1 = x1y1 / x1y1[2];

    int xMin = min(min(min(x0y0[0], x0y1[0]), x1y0[0]), x1y1[0]);
    int xMax = max(max(max(x0y0[0], x0y1[0]), x1y0[0]), x1y1[0]);
    int yMin = min(min(min(x0y0[1], x0y1[1]), x1y0[1]), x1y1[1]);
    int yMax = max(max(max(x0y0[1], x0y1[1]), x1y0[1]), x1y1[1]);

    return BoundingBox(xMin,xMax,yMin,yMax);
}


BoundingBox bboxUnion(BoundingBox B1, BoundingBox B2) {
    // --------- HANDOUT  PS06 ------------------------------
    // Compute the bounding box that tightly bounds the union of B1 an B2.
    return BoundingBox(min(B1.x1, B2.x1),max(B1.x2, B2.x2),min(B1.y1, B2.y1),max(B1.y2, B2.y2));

}


Matrix makeTranslation(BoundingBox B) {
    // --------- HANDOUT  PS06 ------------------------------
    // Compute a translation matrix (as a homography matrix) that translates the
    // top-left corner of B to (0,0).
    Matrix T = Matrix::Zero(3,3);
    T(0,0) = 1.0;
    T(0,1) = 0;
    T(0,2) = -B.x1;
    T(1,0) = 0;
    T(1,1) = 1.0;
    T(1,2) = -B.y1;
    T(2,0) = 0;
    T(2,1) = 0;
    T(2,2) = 1.0;
    return T;
}


Image stitch(const Image &im1, const Image &im2, const CorrespondencePair correspondences[4]) {
    // --------- HANDOUT  PS06 ------------------------------
    // Transform im1 to align with im2 according to the set of correspondences.
    // make sure the union of the bounding boxes for im2 and transformed_im1 is
    // translated properly (use makeTranslation)

    Matrix H = computeHomography(correspondences);
    BoundingBox bbox1 = computeTransformedBBox(im1.width(), im1.height(), H);
    BoundingBox bbox2 = BoundingBox(0, im2.width(), 0, im2.height());
    BoundingBox bbox = bboxUnion(bbox1, bbox2);
    Matrix trans = makeTranslation(bbox);

    Image newImage(bbox.x2-bbox.x1, bbox.y2-bbox.y1, im1.channels());
    applyHomography(im2, trans, newImage, true);
    applyHomography(im1, trans*H, newImage, true);

    return newImage;

}

// debug-useful
Image drawBoundingBox(const Image &im, BoundingBox bbox) {
    // // --------- HANDOUT  PS06 ------------------------------
    //  ________________________________________
    // / Draw me a bounding box!                \
    // |                                        |
    // | "I jumped to my                        |
    // | feet, completely thunderstruck. I      |
    // | blinked my eyes hard. I looked         |
    // | carefully all around me. And I saw a   |
    // | most extraordinary small person, who   |
    // | stood there examining me with great    |
    // | seriousness."                          |
    // \              Antoine de Saint-Exupery  /
    //  ----------------------------------------
    //         \   ^__^
    //          \  (oo)\_______
    //             (__)\       )\/\
    //                 ||----w |
    //                 ||     ||
    Image newImage(im.width(), im.height(), im.channels());
    for (int x = 0; x < newImage.width(); x++) {
      for (int y = 0; y < newImage.height(); y++) {
        for (int c = 0; c < newImage.channels(); c++) {
          newImage(x,y,c) = im(x,y,c);
        }
        if (x == bbox.x1 & x < bbox.x2 & y > bbox.y1 & y < bbox.y2) {
          newImage(x, y, 0) = 0.0;
        } else if (x > bbox.x1 & x == bbox.x2 & y > bbox.y1 & y < bbox.y2) {
          newImage(x, y, 0) = 0.0;
        } else if (x > bbox.x1 & x < bbox.x2 & y == bbox.y1 & y < bbox.y2) {
          newImage(x, y, 0) = 0.0;
        } else if (x > bbox.x1 & x < bbox.x2 & y > bbox.y1 & y == bbox.y2) {
          newImage(x, y, 0) = 0.0;
        }
      }
    }
    return newImage;
}

void applyHomographyFast(const Image &source, const Matrix &H, Image &out, bool bilinear) {
    // // --------- HANDOUT  PS06 ------------------------------
    // Same as apply but change only the pixels of out that are within the
    // predicted bounding box (when H maps source to its new position).
    Matrix HI = H.inverse();
    BoundingBox bbox = computeTransformedBBox(source.width(), source.height(), H);
    int xmin = max(0, bbox.x1);
    int xmax = min(out.width(), bbox.x2 + 1);
    int ymin = max(0, bbox.y1);
    int ymax = min(out.width(), bbox.y2 + 1);

    for (int x = xmin; x < xmax; x++) {
      for (int y = ymin; y < ymax; y++) {
        Vec3f newLoc = HI*Vec3f(x,y,1);
        float newX = newLoc[0] / float(newLoc[2]);
        float newY = newLoc[1] / float(newLoc[2]);
        for (int c = 0; c < out.channels(); c++) {
          if (newX < source.width() & newY < source.height() & newX >= 0 & newY >=0) {
            if (bilinear) {
              out(x,y,c) = interpolateLin(source, newX, newY, c, true);
            } else {
              out(x,y,c) = source(newX, newY, c);
            }
          }
        }
      }
    }
    return;
}
