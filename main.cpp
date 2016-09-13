#include <iostream>
#include <vigra/impex.hxx>
#include <vigra/multi_array.hxx>
#include <vigra/multi_math.hxx>
#include <vigra/stdimage.hxx>
#include <random>
#include <vigra/convolution.hxx>
#include "algorithms/fieldAlgorithms.cpp"
#include "algorithms/deformation.cpp"

#include "utils/Fields.h"
#include "fitting/FitTemplate.cpp"
#include "fitting/Epoch.h"
#include <string.h>
#include <vector>

using namespace vigra;
using namespace vigra::multi_math;

typedef MultiArray<2, RGBValue <unsigned char> > RGBArray;
typedef MultiArray<2, float > FloatArray;
typedef MultiArrayView<2, RGBValue<unsigned char> > RGBView;


int makeFields(char** argv) {
    // read image given as first command-line argument
    vigra::ImageImportInfo imageInfo(argv[2]);  
    FloatArray imageArray(imageInfo.shape());  
    importImage(imageInfo, imageArray);

    //Find the representations for 3 different methods
    std::cout << "By matching with expected Gradients\n";
    Fields fieldsGrad = FieldAlgorithms::fieldsByGradientPattern(imageArray);
    //Draw them
    exportImage(fieldsGrad.valleyField, "./../images/results/valleyGrad.png");
    exportImage(fieldsGrad.peakField, "./../images/results/peakGrad.png");
    FloatArray whatsInPeakGrad = fieldsGrad.intensityField * fieldsGrad.peakField;
    exportImage(whatsInPeakGrad, "./../images/results/whatsInPeakGrad.png");
    FloatArray whatsInValleyGrad = fieldsGrad.intensityField * fieldsGrad.valleyField;
    exportImage(whatsInValleyGrad, "./../images/results/whatsInValleyGrad.png");
    exportImage(fieldsGrad.edgeField, "./../images/results/edges.png");

    std::cout << "By scaling Laplasian of Gaussian\n";
    Fields fieldsLoG = FieldAlgorithms::fieldsByLaplasian(imageArray);
    exportImage(fieldsLoG.valleyField, "./../images/results/valleyLoG.png");
    exportImage(fieldsLoG.peakField, "./../images/results/peakLoG.png");
    FloatArray whatsInPeakLoG = fieldsLoG.intensityField * fieldsLoG.peakField;
    exportImage(whatsInPeakLoG, "./../images/results/whatsInPeakLoG.png");
    FloatArray whatsInValleyLoG = fieldsLoG.intensityField * fieldsLoG.valleyField;
    exportImage(whatsInValleyLoG, "./../images/results/whatsInValleyLoG.png");

    std::cout << "By Erosion, Dialation Morphology\n";
    Fields fieldsMorph = FieldAlgorithms::fieldsByErosionDilation(imageArray);
    exportImage(fieldsMorph.valleyField, "./../images/results/valleyMorph.png");
    exportImage(fieldsMorph.peakField, "./../images/results/peakMorph.png");
    FloatArray whatsInPeakMorph = fieldsMorph.intensityField * fieldsMorph.peakField;
    exportImage(whatsInPeakMorph, "./../images/results/whatsInPeakMorph.png");
    FloatArray whatsInValleyMorph = fieldsMorph.intensityField * fieldsMorph.valleyField;
    exportImage(whatsInValleyMorph, "./../images/results/whatsInValleyMorph.png");
    
    return 0;
}

int exampleLocalization(char** argv)
{
     vigra::ImageImportInfo imageInfo(argv[2]);  
    FloatArray imageArray(imageInfo.shape());  
    importImage(imageInfo, imageArray);
    Fields fieldsGrad = FieldAlgorithms::fieldsByGradientPattern(imageArray);
    //reimport as rgb
    exportImage(fieldsGrad.valleyField, "./../images/results/valleyGrad.png");
    vigra::ImageImportInfo rgbIm("./../images/results/valleyGrad.png");  
    MultiArray<2, RGBValue<UInt8> > rgb_array(rgbIm.shape());
    importImage(rgbIm, rgb_array);
    //This draws you into the picture where blobs got localized
    FieldAlgorithms::localizePOIExample(fieldsGrad.valleyField, rgb_array);
    exportImage(rgb_array, "./../images/results/localizationExample.png");
    return 0;
}

int exampleFit(char** argv)
{
    vigra::ImageImportInfo imageInfo(argv[2]);  
    FloatArray imageArray(imageInfo.shape());  
    importImage(imageInfo, imageArray);
    Fields fieldsGrad = FieldAlgorithms::fieldsByGradientPattern(imageArray);
    //initialize Template with radius 1.
    FitTemplate tmpl(fieldsGrad, 1);
    //Execute the first epoch with these fixed weights
    Epoch::run(tmpl, 1, true, 1, true);
    std::cout << "radius: ";
    std::cout << tmpl.radius;
    std::cout << "\n";
    //We did draw circles according to the radius and its localization here
    FloatArray visualize = tmpl.fields.intensityField;
    exportImage(visualize, "./../images/results/scaleResult.png");
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
		/*svgHandler handler;
		
		char *file = handler.loadSVG(argv[2]);
		Template t = handler.parseSVG(file);
		handler.writeSVG(t, argv[3]);
		
        std::cout << "building templates\n";*/
        return 0;
    }
    else if (strcmp(argv[1], "fields") == 0) 
    {
        std::cout << "calculating fields\n";
        return makeFields(argv);
    }
    else if (strcmp(argv[1], "localization") == 0) 
    {
        return exampleLocalization(argv);
    }
    else if (strcmp(argv[1], "fit") == 0) 
    {
        return exampleFit(argv);
    }
    std::cerr << "Unknown command key\n";
    return 1;
    
}