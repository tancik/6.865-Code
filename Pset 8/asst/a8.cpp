/**
 * Instructions.
 *
 * This file contains a number of empyo function stubs with the problem
 * statements. You need to fill in all details. These problems
 * are designed as extensions to the tutorials in this Pset. Please run
 * each tutorial and go through the code and comments. You are welcome
 * to discuss details on Piazza if you do not understand the tutorials.
 *
 * The main function is in a8_main.cpp. You can modify the contents for
 * your debugging purposes but restore the original contents.
 *
 * Live long and propser.
 */

#include <a8.h>

using namespace Halide;

// This applies a compute_root() schedule to all the Func's that are consumed
// by the calling Func. DO NOT EDIT IT.
void apply_auto_schedule(Func F) {
    map<string,Internal::Function> flist = Internal::find_transitive_calls(F.function());
    flist.insert(std::make_pair(F.name(), F.function()));
    map<string,Internal::Function>::iterator fit;
    for (fit=flist.begin(); fit!=flist.end(); fit++) {
        Func f(fit->second);
        f.compute_root();
        cout << "Warning: applying default schedule to " << f.name() << endl;
    }
    cout << endl;
}

// ---------------------------------- PART 1 -----------------------------------
// For all Halide Func's you write in Part 1, you do not need to worry about
// schedule. However, things will be faster if you just use:
// apply_auto_schedule(final_Result_Func);

Image<uint8_t> SmoothGradNormalized(void) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Use Halide to compute a 512x512 smooth gradient where pixel (x,y) is
    // equal to x+y divided by 1024.
    //
    // HINT: tutorial 1 has a very similar function except that it does not
    // normalize by 1024
    //
    // OUPUT: a single channel output image of size 512x512 (uint8_t)
    //
    // SCHEDULE: use compute_root() on all the Func's you create (i.e.
    // use apply_auto_schedule(Func f).

    Func gradient;
    Var x("x");
    Var y("y");

    gradient(x, y) = cast<uint8_t>(256 * (x + y)/1024.0f);
    apply_auto_schedule(gradient);
    Image<uint8_t> output = gradient.realize(512, 512);
    return output;
}

Image<uint8_t> WavyRGB(void) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Use a Halide Func to compute a wavy RGB image obtained
    // by the formula: output(x,y,c) = (1-c)*cos(x)*cos(y)
    // Schedule: use compute_root() on all the Func's you create.
    //
    // HINT: you need one more dimension than before
    //
    // OUTPUT 512x512 RGB image (uint8_t)
    //
    // SCHEDULE: use compute_root() on all the Func's you create (i.e.
    // use apply_auto_schedule(Func f).
    float pi = 3.1415927;
    Func wavy;
    Var x("x");
    Var y("y");
    Var c("c");
    wavy(x, y, c) = cast<uint8_t>(255/2.0f * (1 + (1 - c) * cos(x * pi / 180.0f) * cos(y * pi / 180.0f)));
    apply_auto_schedule(wavy);
    return wavy.realize(512,512,3);

}

Image<uint8_t> Luminance(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Use Halide to compute a 1-channel luminance image. Use the formula
    // L = 0.3R+0.6G+0.1B
    //
    // INPUT: RGB image
    //
    // OUTPUT: single channel luminance image the same size as the input image
    //
    // SCHEDULE: use compute_root() on all the Func's you create (i.e.
    // use apply_auto_schedule(Func f).

    Func lum;
    Var x("x");
    Var y("y");
    lum(x,y) = cast(UInt(8), 0.3f * input(x,y,0) + 0.6f * input(x,y,1) + 0.1f * (input(x,y,2)));
    apply_auto_schedule(lum);
    return lum.realize(input.width(), input.height());
}

Image<uint8_t> Sobel(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Use Halide to apply a Sobel filter and return the gradient magnitude.
    // Use the following kernels to compute the horizontal and vertical gradients
    //
    //            | -1 0 1 |             | -1 -2 -1 |
    // x kernel = | -2 0 2 |, y kernel = |  0  0  0 |
    //            | -1 0 1 |             |  1  2  1 |
    //
    // INPUT: single channel luminance image
    //
    // OUTPUT: single channel image where each pixel is the magnitude of Sobel x and y filters
    //
    // SCHEDULE: use compute_root() on all the Func's you create (i.e.
    // use apply_auto_schedule(Func f).

    Func xKern("xKern");
    Func yKern("yKern");
    Func sobel("sobel");
    Func clamped("clamped");
    Var x("x");
    Var y("y");
    clamped(x,y) =  cast<float>(input(clamp(x, 0, input.width()-1),clamp(y, 0, input.height()-1)));
    xKern(x,y) = -clamped(x-1,y-1) -2.0f*clamped(x-1,y) -clamped(x-1,y+1) + clamped(x+1,y-1) + 2.0f*clamped(x+1,y) + clamped(x+1,y+1);
    yKern(x,y) = -clamped(x-1,y-1) -2.0f*clamped(x,y-1) -clamped(x+1,y-1) + clamped(x-1,y+1) + 2.0f*clamped(x,y+1) + clamped(x+1,y+1);
    sobel(x,y) = cast<uint8_t>(sqrt(pow(xKern(x,y), 2.0f) + pow(yKern(x,y), 2.0f)));
    apply_auto_schedule(sobel);
    return sobel.realize(input.width(), input.height());
}


// Example code for the equivalent .cpp loop questions
Image<uint8_t> boxSchedule1(Image<uint8_t> input) {
    cout << "Started boxSchedule1" << endl; // DO NOT CHANGE

    // Ignore boundaries: use this width and height
    int w = input.width()-2;
    int h = input.height()-2;

    // Intermediary stages of the pipeline, same size as input
    Image<float> blur_x(input.width(), input.height());
    Image<uint8_t> blur_y(input.width(), input.height());


    // - Schedule to replicate -
    // blur_y.compute_root();
    // blur_x.compute_root();
    // -------------------------

    // Your equivalent loops should go there --------
    // Compute blur_x root
    for (int y=0; y<h; y++) {
        for (int x=0; x<w; x++) {
            cout << "blur_x" << " " << x << " " << y << " " << endl;
            blur_x(x,y) = (static_cast<float>(input(x,y))
                        + static_cast<float>(input(x+1,y))
                        + static_cast<float>(input(x+2,y)))/3.0f;
        }
    }
    // Compute blur_y root
    for (int y=0; y<h; y++) {
        for (int x=0; x<w; x++) {
            cout << "blur_y" << " " << x << " " << y << " " << endl;
            blur_y(x,y) = static_cast<uint8_t>((blur_x(x,y) + blur_x(x,y+1) + blur_x(x,y+2))/3.0f);
        }
    }

    // ----------- till here ------------------------

    cout << "Completed boxSchedule1" << endl; // DO NOT CHANGE
    return blur_y;
}

Image<uint8_t> boxSchedule5(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Write the cpp nested loops corresponding to the 3x3 box schedule 5 in tutorial 6
    // print the order of evaluation.
    // Each time you perform a computation of blur_x or
    // blur_y, put a triplet with the name of the Func ("blur_x" or "blur_y") and
    // the output coordinates x and y. For example, when you compute (50,75) print:
    // blur_x 50 75 ( with exactly one space between each) in a new line.
    //
    // Do not print anything else using cout within the current function.

    cout << "Started boxSchedule5" << endl; // DO NOT CHANGE

    // Ignore boundaries: use this width and height
    int w = input.width()-2;
    int h = input.height()-2;

    // Intermediary stages of the pipeline, same size as input
    Image<float> blur_x(input.width(), input.height());
    Image<uint8_t> blur_y(input.width(), input.height());

    // - Schedule 5 to replicate -
    //   blur_y.compute_root()
    //   blur_x.compute_at(blur_y, x)
    // -------------------------

    for (int y=0; y<h; y++) {
      for (int x=0; x<w; x++) {
        for (int y0=y; y0<y+3; y0++) {
          cout << "blur_x" << " " << x << " " << y0 << " " << endl;
          blur_x(x,y0) = (static_cast<float>(input(x,y0))
                      + static_cast<float>(input(x+1,y0))
                      + static_cast<float>(input(x+2,y0)))/3.0f;
        }
        cout << "blur_y" << " " << x << " " << y << " " << endl;
        blur_y(x,y) = static_cast<uint8_t>((blur_x(x,y) + blur_x(x,y+1) + blur_x(x,y+2))/3.0f);
      }
    }

    // Your equivalent loops should go there --------
    // ----------- till here ------------------------

    cout << "Completed boxSchedule5" << endl; // DO NOT CHANGE
    return blur_y;
}

Image<uint8_t> boxSchedule6(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Write the cpp nested loops corresponding to the 3x3 box schedule 6 in tutorial 6
    // print the order of evaluation.
    // Each time you perform a computation of blur_x or
    // blur_y, put a triplet with the name of the Func ("blur_x" or "blur_y") and
    // the output coordinates x and y. For example, when you compute (50,75) print:
    // blur_x 50 75 ( with exactly one space between each) in a new line.
    //
    // Do not print anything else using cout within the current function.

    cout << "Started boxSchedule6" << endl; // DO NOT CHANGE

    // Ignore boundaries: use this width and height
    int w = input.width()-2;
    int h = input.height()-2;

    // Intermediary stages of the pipeline, same size as input
    Image<float> blur_x(input.width(), input.height());
    Image<uint8_t> blur_y(input.width(), input.height());

    // - Schedule 6 to replicate -
    //   blur_y.tile(x, y, xo, yo, xi, yi, 2, 2)
    //   blur_x.compute_at(blur_y, yo)
    // -------------------------

    // Your equivalent loops should go there --------
    for (int yo=0; yo<h/2; yo++) {
      for (int yi=0; yi<4; yi++){
        int y=yo*2+yi;
        for (int x=0; x<w; x++) {
          cout << "blur_x" << " " << x << " " << y << " " << endl;
          blur_x(x,y) = (static_cast<float>(input(x,y))
                      + static_cast<float>(input(x+1,y))
                      + static_cast<float>(input(x+2,y)))/3.0f;
        }
      }
      for (int xo=0; xo<w/2; xo++) {
        for (int yi = 0; yi<2; yi++) {
          int y=yo*2+yi;
          for (int xi=0; xi<2; xi++) {
            int x=xo*2+xi;
            cout << "blur_y" << " " << x << " " << y << " " << endl;
            blur_y(x,y) = static_cast<uint8_t>((blur_x(x,y) + blur_x(x,y+1) + blur_x(x,y+2))/3.0f);
          }
        }
      }
    }
    // ----------- till here ------------------------

    cout << "Completed boxSchedule6" << endl; // DO NOT CHANGE
    return blur_y;
}


Image<uint8_t> boxSchedule7(Image<uint8_t> input) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Write the cpp nested loops corresponding to the 3x3 box schedule 7 in tutorial 6
    // print the order of evaluation.
    // Each time you perform a computation of blur_x or
    // blur_y, put a triplet with the name of the Func ("blur_x" or "blur_y") and
    // the output coordinates x and y. For example, when you compute (50,75) print:
    // blur_x 50 75 ( with exactly one space between each) in a new line.
    //
    // Do not print anything else using cout within the current function.

    cout << "Started boxSchedule7" << endl; // DO NOT CHANGE

    // Ignore boundaries: use this width and height
    int w = input.width()-2;
    int h = input.height()-2;

    // Intermediary stages of the pipeline, same size as input
    Image<float> blur_x(input.width(), input.height());
    Image<uint8_t> blur_y(input.width(), input.height());

    // - Schedule 7 to replicate -
    //   blur_y.split(x, xo, xi, 2)
    //   blur_x.compute_at(blur_y, y)
    // -------------------------

    // Your equivalent loops should go there --------
    for (int y=0; y<h; y++) {
      for (int ytemp=y; ytemp<y+3; ytemp++) {
        for (int x=0; x<w; x++) {
          cout << "blur_x" << " " << x << " " << ytemp << " " << endl;
          blur_x(x,ytemp) = (static_cast<float>(input(x,ytemp))
                      + static_cast<float>(input(x+1,ytemp))
                      + static_cast<float>(input(x+2,ytemp)))/3.0f;
        }
      }
      for (int xo=0; xo<w/2; xo++) {
        for (int xi=0; xi<2; xi++) {
          int x=xo*2+xi;
          cout << "blur_y" << " " << x << " " << y << " " << endl;
          blur_y(x,y) = static_cast<uint8_t>((blur_x(x,y) + blur_x(x,y+1) + blur_x(x,y+2))/3.0f);
        }
      }
    }
    // ----------- till here ------------------------

    cout << "Completed boxSchedule7" << endl; // DO NOT CHANGE
    return blur_y;
}


// ---------------------------------- PART 2 -----------------------------------
// For all the problems in Part 2, yo are expected to write schedules. You will
// score points for faster schedules. You should also report the machine used
// for testing in the form accompanying this Pset. Each problem is supposed to
// return a Halide Func that computes the result in uint8_t. All intermediate
// operations should be performed in float for Gaussian and unsharp mask.
//
// First get the algorithm right and make sure it is generating the
// correct output. Use apply_auto_schedule() during this phase. Once you
// are confident of your result, remove apply_auto_schedule() and write
// your own schedule. You can set the macro AUTO_SCHEDULE to 0 or 1 to easily
// alternate between the default and updated schedules.
//
// Hint: your schedule should beat apply_auto_schedule()


Func LocalMax(Image<uint8_t> input, int window) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Compute the local maximum in a window of given size centered at the pixel.
    // Return white color if it's a local maximum and black otherwise.
    // Run your function with autoschedule as you write the algorithm, then
    // write your own (better) schedule.
    //
    // Handle boundary conditions by clamping at boundaries (see tutorials)
    //
    // INPUT: single channel luminance image
    // OUTPUT: Halide Func that computes the localMax

    // output Func
    Func LM("LocalMax");
    Func max("max");
    Func clamped("clamped");

    // useful vars
    Var x("x"), y("y");
    Var xi("xi"), yi("yi");
    Var xo("xo"), yo("yo");

    // Clamp the input
    clamped(x,y) =  input(clamp(x, 0, input.width()-1),clamp(y, 0, input.height()-1));

    // Compute the local maximum in a window [-window/2;window/2]x[-window/2;window/2]
    // around each pixel.
    RDom r(-window/2, window, -window/2, window);

    max(x,y) = cast<uint8_t>(0);
    max(x,y) = select(clamped(x+r.x, y+r.y) > max(x,y), clamped(x+r.x, y+r.y), max(x,y));

    // Write the output local Max: It should be 255 (uint8_t images) if the
    // curent pixel is the local maximum, 0 otherwise
    LM(x,y) = cast<uint8_t>(select(max(x,y) == input(x,y), 255, 0));

    // Schedule your pipeline

    // Debug to html

    // Return the output Func (cast it to uint8_t)

#define AUTO_SCHEDULE 1
#if AUTO_SCHEDULE
    apply_auto_schedule(LM);
#else
    LM.tile(x,y,xo,yo,xi,yi,256,32)
      .parallel(yo)
      .vectorize(xi, 16);
    max.compute_at(LM, xo)
       .vectorize(x, 16);
#endif

    // Debug HTML
    // Buffer b(type_of<uint8_t>(),input.height(),input.width());
    // LM.compile_to_lowered_stmt("Output/LocaLM.html", {b}, HTML);

    return LM;
}


Func Gaussian(Image<uint8_t> input, float sigma, float truncate) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Perform 2D gaussian blur. use what you learnt in previous Pset about
    // fast Gaussian blur algorithm. The Gaussians have variance sigma and the
    // kernels are truncated at trunc*sigma on both sides. We give you the
    // kernel.
    //
    // Use the sum() sugar (see tutorials)
    // First generate the kernel as a Halide Func
    // Handle boundary conditions
    //
    // INPUT: single channel luminance image
    //
    // OUTPUT: Func that computes Gaussian blur

    Func GB("Gaussian");

    Var x("x"), y("y");
    Var xi("xi"), yi("yi");
    Var xo("xo"), yo("yo");

    int radius = sigma * truncate;
    int fwidth = 2 * radius + 1;

    // compute the Gaussian kernel
    Func GKernelUnNorm("GKernelUnnorm");
    Func GKernelSum   ("GKernelSum");
    Func GKernel      ("GKernel");
    RDom rx(0, fwidth);
    GKernelUnNorm(x) = exp(-((x-radius)*(x-radius))/(2.0f*sigma*sigma));
    GKernelSum   (x) = sum(GKernelUnNorm(rx));
    GKernel      (x) = GKernelUnNorm(x)/GKernelSum(0);

    // Schedule the kernel to be computed root, so we do not recompute it every
    // time we use it.
    GKernelUnNorm.compute_root();
    GKernelSum   .compute_root();
    GKernel      .compute_root();
    Image<float> kernel = GKernel.realize(fwidth);

    // Clamp the image (boundary conditions)
    Func clamped("clamp");
    Func blur_x ("blur_x");
    Func blur_y ("blur_y");
    clamped(x,y) =  cast<float>(input(clamp(x, 0, input.width()-1), clamp(y, 0, input.height()-1)));
    // Blur in x
    blur_x(x,y) += clamped(x+rx - fwidth/2, y) * kernel(rx);
    // Blur in y
    blur_y(x,y) += blur_x(x, y+rx - fwidth/2) * kernel(rx);
    GB(x,y) = cast<uint8_t>(blur_y(x,y));
    // Schedule your pipeline

    // Debug to html

    // Return the output Func (cast it to uint_8t)

#define AUTO_SCHEDULE 0
#if AUTO_SCHEDULE
    apply_auto_schedule(GB);
#else
    GB.tile(x,y,xo,yo,xi,yi,256,128)
      .parallel(yo)
      .vectorize(xi, 16);
    blur_y.compute_at(GB, xo);
    blur_x.compute_at(GB, yo)
       .vectorize(x, 16);
#endif

    // Debug HTML
    // Buffer b(type_of<uint8_t>(),input.height(),input.width());
    // GB.compile_to_lowered_stmt("Output/Gaussian.html", {b}, HTML);

    return GB;
}

Func UnsharpMask(Image<uint8_t> input, float sigma, float truncate, float strength) {
    // // --------- HANDOUT  PS08 ------------------------------
    // Implement Unsharp Mask. Use a Gaussian of standard deviation sigma to extract
    // a lowpassed version of the image. Subtract that lowpassed version from the
    // original image to produce a highpassed version of the image and then add the
    // highpassed version back to it strength times.
    //
    // Handle boundary conditions
    // Use the Gaussian above for blur
    // You may want to write a new Gaussian with a new schedule because it may be
    // possible to use a different schedule for Gaussian blur such that it works
    // better with subsequent stages
    //
    // INPUT: single channel luminance image
    // OUTPUt: Halide Func that computes the localMax
    //
    // return Func("f");

    Func GB("USM");

    Var x("x"), y("y");
    Var xi("xi"), yi("yi");
    Var xo("xo"), yo("yo");

    int radius = sigma * truncate;
    int fwidth = 2 * radius + 1;

    // compute the Gaussian kernel
    Func GKernelUnNorm("GKernelUnnorm");
    Func GKernelSum   ("GKernelSum");
    Func GKernel      ("GKernel");
    RDom rx(0, fwidth);
    GKernelUnNorm(x) = exp(-((x-radius)*(x-radius))/(2.0f*sigma*sigma));
    GKernelSum   (x) = sum(GKernelUnNorm(rx));
    GKernel      (x) = GKernelUnNorm(x)/GKernelSum(0);

    // Schedule the kernel to be computed root, so we do not recompute it every
    // time we use it.
    GKernelUnNorm.compute_root();
    GKernelSum   .compute_root();
    GKernel      .compute_root();
    Image<float> kernel = GKernel.realize(fwidth);

    // Clamp the image (boundary conditions)
    Func clamped("clamp");
    Func blur_x ("blur_x");
    Func blur_y ("blur_y");
    clamped(x,y) =  cast<float>(input(clamp(x, 0, input.width()-1), clamp(y, 0, input.height()-1)));
    // Blur in x
    blur_x(x,y) += clamped(x+rx - fwidth/2, y) * kernel(rx);
    // Blur in y
    blur_y(x,y) += blur_x(x, y+rx - fwidth/2) * kernel(rx);
    GB(x,y) = cast<uint8_t>(strength * input(x,y) + (1 -strength) * blur_y(x,y));

#define AUTO_SCHEDULE 0
#if AUTO_SCHEDULE
    apply_auto_schedule(GB);
#else
    GB.tile(x,y,xo,yo,xi,yi,256,128)
      .parallel(yo)
      .vectorize(xi, 16);
    blur_y.compute_at(GB, xo);
    blur_x.compute_at(GB, yo)
       .vectorize(x, 16);
#endif
    return GB;
}
