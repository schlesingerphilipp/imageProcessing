#include <iostream>
#include <vigra/impex.hxx>
#include <vigra/multi_array.hxx>
#include <vigra/multi_math.hxx>
#include <vigra/stdimage.hxx>
#include <random>
#include <vigra/convolution.hxx>
#include "algorithms/fieldAlgorithms.cpp"
#include "utils/Point.h"

using namespace vigra;
using namespace vigra::multi_math;

typedef MultiArray<2, RGBValue <unsigned char> > RGBArray;
typedef MultiArray<2, float > FloatArray;
typedef MultiArrayView<2, RGBValue<unsigned char> > RGBView;



void  addNormalNoise(FloatArray &image, float sigma) {
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.0,sigma);
    for (float &pixel : image) {
        float x = (distribution(generator) + pixel);
        pixel = (x <= 255) ?  ((x >= 0) ? x : 0) : 255;
    }
}


int main(int argc, char** argv)
{
    if (argc <= 1) {
        std::cerr << "Please give me a picture as an argument!\n";
        return 1;
    }
    // read image given as first command-line argument
    vigra::ImageImportInfo imageInfo(argv[1]);

    
    FloatArray imageArray(imageInfo.shape());  
    // copy image data from file into array
    importImage(imageInfo, imageArray);
    FloatArray valley = FieldAlgorithms::valley(imageArray);
    //Point p = FieldAlgorithms::localize(valley);
    exportImage(valley, argv[2]);
    return 0;
}