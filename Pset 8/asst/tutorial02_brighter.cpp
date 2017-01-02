// Halide tutorial lesson 2.

// This lesson demonstrates how to pass in input images.

#include <iostream>
#include <cmath>
#include <Halide.h>
#include <image_io.h>

using std::cout;
using std::endl;

using namespace Halide;
using namespace Halide::Tools;

int main(int argc, char** argv)
{
    // This program defines a single-stage imaging pipeline that
    // brightens an image.

    // First we'll load the input image we wish to brighten.
    // We'll use imageIO to get a numpy array from a PNG image
    Image<uint8_t> input = load_image("Input/rgb.png");

    // Next we declare our Func object that represents our one pipeline
    // stage.
    Func brighter;

    // Our Func will have three arguments, representing the position
    // in the image and the color channel. Halide treats color
    // channels as an extra dimension of the image, just like in numpy
    // let's declare the corresponding Vars before we can use them
    Var x, y, c;

    // Normally we'd probably write the whole function definition on
    // one line. Here we'll break it apart so we can explain what
    // we're doing at every step.

    // For each pixel of the input image, define an Expr for the input value
    // again note the square brackets
    Expr value= input(x, y, c);

    // Cast it to a floating point value.
    value = cast<float>(value);

    // Multiply it by 1.5 to brighten it.
    value = value * 1.5f;

    // Clamp it to be (0,255), so we don't get overflow when we
    // cast it back to an 8-bit unsigned int.
    value = clamp(value, 0.0f, 255.0f);

    // Cast it back to an 8-bit unsigned integer.
    value = cast<uint8_t>(value);

    // Finally define the function.
    brighter(x, y, c) = value;

    // The equivalent one-liner to all of the above is:
    //
    // brighter(x, y, c) = input(x, y, c) * 1.5
    //

    // Remember. All we've done so far is build a representation of a
    // Halide program in memory. We haven't actually processed any
    // pixels yet. We haven't even compiled that Halide program yet.

    // So now we'll realize the Func. The size of the output image
    // should match the size of the input image. If we just wanted to
    // brighten a portion of the input image we could request a
    // smaller size. If we request a larger size Halide will throw an
    // error at runtime telling us we're trying to read out of bounds
    // on the input image.
    Image<uint8_t> output = brighter.realize(input.width(), input.height(), input.channels());

    save_image(output, "Output/tut2_brighter.png");

    cout << "Success!\n" << endl;

    return EXIT_SUCCESS;
}
