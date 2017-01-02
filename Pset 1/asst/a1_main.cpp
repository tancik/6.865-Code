#include "a1.h"
#include <iostream>

using namespace std;


// This is a way for you to test your functions.
// We will only grade the contents of a1.cpp and Image.cpp
int main() {

    Image im("./Input/flower.png");
    Image newImage = grayworld(im);
    newImage.write("./Output/flower.png");

    // // Example
    // Image im("./Input/castle_small.png");
    // std::vector<Image> LC = spanish(im);
    // LC[0].write("./Output/castle_color.png");
    // LC[1].write("./Output/castle_grey.png");
}
