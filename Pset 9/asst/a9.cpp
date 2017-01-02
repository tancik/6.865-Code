#include <a9.h>
#include <algorithm>
#include "timing.h"

using namespace Halide;

Func HarrisCorners(const Image<float> &input,
    int schedule_index,
    vector<int> schedule_parameters,
    float sigmaG,
    float factorSigma,
    float truncate,
    float k,
    float thresh,
    int maxiDiam
    )
{
    // --------- HANDOUT  PS08 ------------------------------
    // Re-implement the Harris Corner detector using Halide. By fusing
    // different stages of the processing pipeline and properly scheduling your
    // opertations, you should be able to get a drastic speed-up over the
    // reference implementation given in 'reference_implementation.cpp'.
    //
    // Make sure you debug your intermediate Func (by
    // realizing them over some domain for example). You can use the
    // normalize_image helper function to remap the pixel value to the [0,1]
    // range, valid for display.
    //
    // Start by building a correct implementation of the algorithm,
    // Then walk your way up to a fast implementation by trying
    // various schedules.
    //
    // Return the output Func, properly scheduled. Chek the reference
    // implementation to see how the input parameters are used.
    //
    // You can use scheule_index to switch between different schedules and
    // compare them, this will come in handy when you implement an autotuner
    // (6.865). You can also pass schedule parameters as a vector of ints.


    // --------- Algorithm ----------------------------------
    Func output("output");
    Func clamped("clamped");
    Func lum("luminance");
    Var x("x"), y("y"), c("c");
    Var xi("xi"), yi("yi");
    Var xo("xo"), yo("yo");

    clamped = BoundaryConditions::repeat_edge(input);
    lum(x,y) = 0.3f * clamped(x,y,0) + 0.6f * clamped(x,y,1) + 0.1f * (clamped(x,y,2));

    Func LumGKernelUnNorm("LumGKernelUnnorm");
    Func LumGKernelSum   ("LumGKernelSum");
    Func LumGKernel      ("LumGKernel");
    int offset = int(ceil(sigmaG*truncate));
    int radius = sigmaG * truncate;
    int fsize = 2*offset+1;
    RDom rx(0, fsize);
    LumGKernelUnNorm(x) = exp(-((x-radius)*(x-radius))/(2.0f*sigmaG*sigmaG));
    LumGKernelSum   (x) = sum(LumGKernelUnNorm(rx));
    LumGKernel      (x) = LumGKernelUnNorm(x)/LumGKernelSum(0);

    // Schedule the kernel to be computed root, so we do not recompute it every
    // time we use it.
    LumGKernelUnNorm.compute_root();
    LumGKernelSum   .compute_root();
    LumGKernel      .compute_root();
    Image<float> lumKernel = LumGKernel.realize(fsize);

    Func lumBlur("blurred_luminance");
    Func lumBlur_x ("lumblur_x");
    Func helper_lumBlur_x("helper_lumBlur_x");
    Func lumBlur_y ("lumblur_y");
    // Blur in x
    lumBlur_x(x,y) += lum(x+rx - fsize/2, y) * lumKernel(rx);
    helper_lumBlur_x(x,y) = lumBlur_x(x,y);
    // Blur in y
    lumBlur_y(x,y) += helper_lumBlur_x(x, y+rx - fsize/2) * lumKernel(rx);
    lumBlur(x,y) = lumBlur_y(x,y);

    Func sobelFilter_x("sobelFilter_x");
    Func sobelFilter_y("sobelFilter_y");
    sobelFilter_x(x,y) = -lumBlur(x-1,y-1) -2.0f*lumBlur(x-1,y) -lumBlur(x-1,y+1) + lumBlur(x+1,y-1) + 2.0f*lumBlur(x+1,y) + lumBlur(x+1,y+1);
    sobelFilter_y(x,y) = -lumBlur(x-1,y-1) -2.0f*lumBlur(x,y-1) -lumBlur(x+1,y-1) + lumBlur(x-1,y+1) + 2.0f*lumBlur(x,y+1) + lumBlur(x+1,y+1);

    Func tensor("tensor");

    tensor(x,y,c) = 0.0f;
    tensor(x,y,0) = sobelFilter_x(x,y)*sobelFilter_x(x,y);
    tensor(x,y,1) = sobelFilter_x(x,y)*sobelFilter_y(x,y);
    tensor(x,y,2) = sobelFilter_y(x,y)*sobelFilter_y(x,y);

    Func TenseGKernelUnNorm("TenseGKernelUnNorm");
    Func TenseGKernelSum   ("TenseGKernelSum");
    Func TenseGKernel      ("TenseGKernel");
    offset = int(ceil(sigmaG*factorSigma*truncate));
    radius = sigmaG * factorSigma * truncate;
    fsize = 2*offset+1;
    RDom rx2(0, fsize);
    TenseGKernelUnNorm(x) = exp(-((x-radius)*(x-radius))/(2.0f*sigmaG*sigmaG*factorSigma*factorSigma));
    TenseGKernelSum   (x) = sum(TenseGKernelUnNorm(rx));
    TenseGKernel      (x) = TenseGKernelUnNorm(x)/TenseGKernelSum(0);

    // Schedule the kernel to be computed root, so we do not recompute it every
    // time we use it.
    TenseGKernelUnNorm.compute_root();
    TenseGKernelSum   .compute_root();
    TenseGKernel      .compute_root();
    Image<float> tenseKernel = TenseGKernel.realize(fsize);

    Func tenseBlur("tenseBlur");
    Func tenseBlur_x ("tenseBlur_x");
    Func helper_tenseBlur_x("helper_tenseBlur_x");
    Func tenseBlur_y ("tenseBlur_y");
    // Blur in x
    tenseBlur_x(x,y,c) += tensor(x+rx2 - fsize/2, y, c) * tenseKernel(rx2);
    helper_tenseBlur_x(x,y,c) = tenseBlur_x(x,y,c);
    // Blur in y
    tenseBlur_y(x,y,c) += helper_tenseBlur_x(x, y+rx2 - fsize/2, c) * tenseKernel(rx2);
    tenseBlur(x,y,c) = tenseBlur_y(x,y,c);

    Func response("response");
    response(x,y) = tenseBlur(x,y,0) * tenseBlur(x,y,2) - tenseBlur(x,y,1) * tenseBlur(x,y,1) - k * ((tenseBlur(x,y,0) + tenseBlur(x,y,2)) * (tenseBlur(x,y,0) + tenseBlur(x,y,2)));

    Func maxifilt("maxifilt");
    Func helper_maxifilt("helper_maxifilt");
    RDom rwin(-maxiDiam/2, maxiDiam, -maxiDiam/2, maxiDiam);
    maxifilt(x,y) = response(x,y);
    maxifilt(x,y) = select(response(x+rwin.x, y+rwin.y) > maxifilt(x,y), response(x+rwin.x, y+rwin.y), maxifilt(x,y));
    helper_maxifilt(x,y) = maxifilt(x,y);

    output(x,y) = select(maxifilt(x,y) == response(x,y) && maxifilt(x,y) > thresh, 1.0f, 0.0f);
    // --------- Schedule -----------------------------------
    // Useful structure for you to switch between various schedule
    // pass schedule_index as argument to your HarrisCorners.
    // =========
    // IMPORTANT
    // =========
    // At the end, put your best schedule in the "schedule_index=0" case.
    switch(schedule_index){
        case 0:
        {
          output.tile(x,y,xo,yo,xi,yi,256,128)
            .parallel(yo)
            .vectorize(xi, 16);
          helper_maxifilt.compute_at(output, xo).vectorize(x,16);
          response.compute_at(output, xo).vectorize(x,16);
          helper_tenseBlur_x.compute_at(output, xo);
          tenseBlur.compute_at(output, xo).vectorize(x,16);
          TenseGKernel.compute_root();
          TenseGKernelSum.compute_root();
          TenseGKernelUnNorm.compute_root();
          tensor.compute_at(output, xo).vectorize(x,16);
          sobelFilter_y.compute_at(output, xo).vectorize(x,16);
          sobelFilter_x.compute_at(output, xo).vectorize(x,16);
          helper_lumBlur_x.compute_at(output, xo).vectorize(x,16);
          lumBlur.compute_at(output, xo).vectorize(x,16);
          LumGKernel.compute_root();
          LumGKernelSum.compute_root();
          LumGKernelUnNorm.compute_root();
          lum.compute_root().vectorize(x,16);
          break;
        }

        case 1:
        {
          int tileWidth  = schedule_parameters[0];
          int tileHeight = schedule_parameters[1];
          int vecLevel = schedule_parameters[2];
          output.tile(x,y,xo,yo,xi,yi,tileWidth,tileHeight)
            .parallel(yo)
            .vectorize(xi, vecLevel);
          helper_maxifilt.compute_at(output, xo).vectorize(x,vecLevel);
          response.compute_at(output, xo).vectorize(x,vecLevel);
          helper_tenseBlur_x.compute_at(output, xo);
          tenseBlur.compute_at(output, xo).vectorize(x,vecLevel);
          TenseGKernel.compute_root();
          TenseGKernelSum.compute_root();
          TenseGKernelUnNorm.compute_root();
          tensor.compute_at(output, xo).vectorize(x,vecLevel);
          sobelFilter_y.compute_at(output, xo).vectorize(x,vecLevel);
          sobelFilter_x.compute_at(output, xo).vectorize(x,vecLevel);
          helper_lumBlur_x.compute_at(output, xo).vectorize(x,vecLevel);
          lumBlur.compute_at(output, xo).vectorize(x,vecLevel);
          LumGKernel.compute_root();
          LumGKernelSum.compute_root();
          LumGKernelUnNorm.compute_root();
          lum.compute_root().vectorize(x,vecLevel);
          break;
        }

        case 2: // vectorize everything
        {
          int tileWidth  = schedule_parameters[0];
          int tileHeight = schedule_parameters[1];
          int vecLevel = schedule_parameters[2];
          output.tile(x,y,xo,yo,xi,yi,tileWidth,tileHeight)
            .parallel(yo)
            .vectorize(xi, vecLevel);
          helper_maxifilt.compute_at(output, xo).vectorize(x,vecLevel);
          response.compute_at(output, xo).vectorize(x,vecLevel);
          helper_tenseBlur_x.compute_at(output, xo).vectorize(x,vecLevel);
          tenseBlur.compute_at(output, xo).vectorize(x,vecLevel);
          TenseGKernel.compute_root().vectorize(x,vecLevel);
          TenseGKernelSum.compute_root().vectorize(x,vecLevel);
          TenseGKernelUnNorm.compute_root().vectorize(x,vecLevel);
          tensor.compute_at(output, xo).vectorize(x,vecLevel);
          sobelFilter_y.compute_at(output, xo).vectorize(x,vecLevel);
          sobelFilter_x.compute_at(output, xo).vectorize(x,vecLevel);
          helper_lumBlur_x.compute_at(output, xo).vectorize(x,vecLevel);
          lumBlur.compute_at(output, xo).vectorize(x,vecLevel);
          LumGKernel.compute_root().vectorize(x,vecLevel);
          LumGKernelSum.compute_root().vectorize(x,vecLevel);
          LumGKernelUnNorm.compute_root().vectorize(x,vecLevel);
          lum.compute_root().vectorize(x,vecLevel);
          break;
        }

        case 3: // Only vectorize luminosity and tiles
        {
          int tileWidth  = schedule_parameters[0];
          int tileHeight = schedule_parameters[1];
          int vecLevel = schedule_parameters[2];
          output.tile(x,y,xo,yo,xi,yi,tileWidth,tileHeight)
            .parallel(yo)
            .vectorize(xi, vecLevel);
          helper_maxifilt.compute_at(output, xo);
          response.compute_at(output, xo);
          helper_tenseBlur_x.compute_at(output, xo);
          tenseBlur.compute_at(output, xo);
          TenseGKernel.compute_root();
          TenseGKernelSum.compute_root();
          TenseGKernelUnNorm.compute_root();
          tensor.compute_at(output, xo);
          sobelFilter_y.compute_at(output, xo);
          sobelFilter_x.compute_at(output, xo);
          helper_lumBlur_x.compute_at(output, xo);
          lumBlur.compute_at(output, xo);
          LumGKernel.compute_root();
          LumGKernelSum.compute_root();
          LumGKernelUnNorm.compute_root();
          lum.compute_root().vectorize(x,vecLevel);
          break;
        }

        default:
        {
            // The default, all root schedule
            // run HarrisCorners(input, -1) for example to get there.
            apply_auto_schedule(output);
            break;
        }
    }

    return output;
}

// -------------------------------------------------------------------------------------
// This applies a compute_root() schedule to all the Func's that are consumed
// by the calling Func. DO NOT EDIT IT.
void apply_auto_schedule(Func F) {
    map<string,Internal::Function> flist = Internal::find_transitive_calls(F.function());
    flist.insert(std::make_pair(F.name(), F.function()));
    map<string,Internal::Function>::iterator fit;
    for (fit=flist.begin(); fit!=flist.end(); fit++) {
        Func f(fit->second);
        f.compute_root();
        // cout << "Warning: applying default schedule to " << f.name() << endl;
    }
    cout << endl;
}
// -------------------------------------------------------------------------------------

void autoschedule_harris(const Image<float> &input) {
    // --------- HANDOUT  PS08 ------------------------------
    // An example autotuner, that does not do much yet... Build your own !
    //

    cout << "** Autotuning begins **" << endl
         << "=======================" << endl;
    int w = input.width();
    int h = input.height();
    int maxTileX = floor(log2(w));
    int maxTileY = floor(log2(h));

    vector<float> timings;
    vector<int> schedule_idxs;
    vector<vector<int >> schedule_params;

    for (int idx = 1; idx <= 3; idx++){
      for (int i = 4; i <= maxTileX; i++) {
        for (int j = 4; j <= maxTileY; j++) {
          for (int k = 3; k < 5; k++) {
            vector<int> params;
            params.push_back(pow(2,i));
            params.push_back(pow(2,j));
            params.push_back(pow(2,k));

            Func f = HarrisCorners(input, idx, params);

            cout << endl
                 << "---------------------------------------" << endl;
            cout
                << "* Autoschedule | " <<  idx << endl;

            cout << "  Params: " ;
            for (int i = 0; i < params.size(); ++i) {
                cout << params[i] << " ";
            }
            cout << endl;

            float time = profile(f, w, h);
            timings.push_back(time);
            schedule_idxs.push_back(idx);
            schedule_params.push_back(params);

            cout << endl
                 << "---------------------------------------" << endl;
            }
         }
       }
    }

    auto mini = std::min_element(timings.begin(), timings.end());
    int min_idx = mini-timings.begin();
    cout << endl
         << "==========================" << endl
         << "** Autotuning complete **" << endl
         << "   - best schedule: " << min_idx << endl
         << "   - time: " << timings[min_idx] << " ms" << endl
         << "   - params: ";
    for (int i = 0; i < schedule_params[min_idx].size(); ++i) {
        cout << schedule_params[min_idx][i] << " ";
    }
    cout << endl;
}
