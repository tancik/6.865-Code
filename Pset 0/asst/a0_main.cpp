#include "a0.h"

using namespace std;

// This is a way for you to test your functions.
// We will only grade the contents of a0.cpp and Image.cpp
int main() {
    // --------- HANDOUT  PS00 ------------------------------
    cout << "Congratulations, you have compiled the starter code!" << endl;

    // helloworld(4.5, 6.6);

    // Example Test
    // Read an Image
    Image im("./Input/Boston_low_contrast.png");

    // // Write an Image
    cout << "Image size: " << im.width() << ", " << im.height() << ", " << im.channels() << endl;
    im.write("./Output/SameAsInput.png");
}
