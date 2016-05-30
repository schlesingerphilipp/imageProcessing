#include <iostream>
#include <vigra/impex.hxx>
#include <vigra/multi_array.hxx>
#include <vigra/multi_math.hxx>
#include <vigra/stdimage.hxx>
#include <random>
#include <vigra/convolution.hxx>


using namespace vigra;
using namespace vigra::multi_math;

typedef MultiArray<2, RGBValue <unsigned char> > RGBArray;
typedef MultiArray<2, float > FloatArray;
typedef MultiArrayView<2, RGBValue<unsigned char> > RGBView;

int printImageInformation(ImageImportInfo imageInfo) {
    std::cout << "Image information:\n";
    std::cout << "  file format: " << imageInfo.getFileType() << std::endl;
    std::cout << "  width:       " << imageInfo.width() << std::endl;
    std::cout << "  height:      " << imageInfo.height() << std::endl;
    std::cout << "  pixel type:  " << imageInfo.getPixelType() << std::endl;
    std::cout << "  color image: ";
    if (imageInfo.isColor())    std::cout << "yes (";
    else                        std::cout << "no  (";
    std::cout << "number of channels: " << imageInfo.numBands() << ")\n";
    
    return 0;
}

void eatApple(MultiArray<2, RGBValue<unsigned char> > &imageArray) {
    
    Shape2  upperLeft  = Shape2(0, imageArray.height() / 2),
            lowerRight = Shape2(imageArray.width() / 2, imageArray.height());
    
    // create subimage around center for output
    MultiArrayView<2, RGBValue<unsigned char> > subimage = imageArray.subarray(upperLeft, lowerRight);
    for (int i = 0; i < subimage.width();) {
        Shape2  upperLeft2  = Shape2(i, i);
        i += subimage.width() / 8;
        Shape2  lowerRight2 = Shape2(i,subimage.height());
        MultiArrayView<2, RGBValue<unsigned char> > subsubimage = subimage.subarray(upperLeft2, lowerRight2);
        subsubimage = 255;
    }
}

MultiArray<2,UInt8> invert(MultiArray<2, RGBValue<unsigned char> > &imageArray) {
    RGBValue<unsigned char> rgb(10,80,0);
    imageArray = rgb-imageArray;
    
    MultiArray<2,UInt8> green_channel(imageArray.bindElementChannel(1));
    return green_channel;
}

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
  
    
  
    // print some information:
    printImageInformation(imageInfo);
    
    //TUTORIAL--------
    // instantiate array for image data
    FloatArray imageArray(imageInfo.shape());
    FloatArray tmpimageArray(imageInfo.shape());
    FloatArray minusArray(imageInfo.shape());
 
    
    // copy image data from file into array
    importImage(imageInfo, imageArray);
 
    
    // reflectImage(imageArray, newImageArray, vertical);
    
//    for (int i = 0; i < imageInfo.width(); i++) {
 //       tmpimageArray.bind<0>(i) = imageArray.bind<0>(imageInfo.width()-1-i);
  //  }
    //addNormalNoise(imageArray, 10.0);
    gaussianSmoothing(imageArray, imageArray, 2.0, 2.0);
    gaussianSmoothing(imageArray, tmpimageArray, 10.0, 10.0);
    minusArray = tmpimageArray - imageArray;
    exportImage(tmpimageArray, ImageExportInfo("mutant_apple3.jpg").setCompression("JPEG QUALITY=100"));
    exportImage(minusArray, ImageExportInfo("mutant_apple2.jpg").setCompression("JPEG QUALITY=100"));
    
    
    
    vigra::ImageImportInfo imageInfo2("mutant_apple2.jpg");
    
    
    
    printImageInformation(imageInfo2);
    
    
    

    return 0;
}