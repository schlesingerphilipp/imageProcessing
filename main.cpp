#include <iostream>
#include <vigra/impex.hxx>
#include <vigra/multi_array.hxx>
#include <vigra/multi_math.hxx>
#include <vigra/stdimage.hxx>
#include <random>
#include <vigra/convolution.hxx>
#include "algorithms/fieldAlgorithms.cpp"
#include "utils/Point.h"
#include <string.h>
#include <vector>


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

int makeFields(char** argv) {
    // read image given as first command-line argument
    vigra::ImageImportInfo imageInfo(argv[2]);

    
    FloatArray imageArray(imageInfo.shape());  
    // copy image data from file into array
    importImage(imageInfo, imageArray);
    std::vector<FloatArray> fields = FieldAlgorithms::fieldsByErosionDilation(imageArray);
    //Point p = FieldAlgorithms::localize(valley);
    exportImage(fields[0], "./../images/results/valley.png");
    exportImage(fields[1], "./../images/results/peak.png");
    exportImage(fields[2], "./../images/results/edge.png");


    return 0;
}


int main(int argc, char** argv)
{
    if (argc <= 1) {
        std::cerr << "Please give me a key as an command!\n";
        return 1;
    }
    if (strcmp(argv[1], "svg") == 0) 
    {
        //Zuemra code here
        std::cout << "building templates\n";
        return 0;
    }
    else if (strcmp(argv[1], "fields") == 0) 
    {
        std::cout << "calculating fields\n";
        return makeFields(argv);
    }
    std::cerr << "Unknown command key\n";
    return 1;
    
}