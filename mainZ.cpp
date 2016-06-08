#include <iostream>
#include <math.h>
#include <stdio.h>
#include <vigra/impex.hxx>
#include <vigra/multi_array.hxx>
#include <vigra/multi_math.hxx>
#include <vigra/stdimage.hxx>
#include <random>
#include <vigra/convolution.hxx>
#include <vigra/edgedetection.hxx>


using namespace vigra;
using namespace vigra::multi_math;

typedef MultiArray<2, RGBValue <unsigned char> > RGBArray;
typedef MultiArray<2, float > FloatArray;
typedef MultiArrayView<2, RGBValue<unsigned char> > RGBView;
typedef MultiArray<2,UInt8> StdArray;
typedef MultiArrayView<2,UInt8> StdArrayView;

void cutImage(StdArray &image, int size) {
	Shape2 upperLeft = Shape2(0, 0), lowerRight = Shape2(size, size);
	
	image = image.subarray(upperLeft, lowerRight);
}

double convolve (Point2D pixel) {
	return 0.0;
}

void extractValley(StdArray &image) {
	
	int step = 5;
	for (int k = 0; k < image.height(); k = k + step) {
		for (int i = 0; i < image.width(); i = i + step) {
	
			Shape2 upperLeft = Shape2(i, k), lowerRight = Shape2(i + step, k + step);
	
			StdArrayView subImage = image.subarray(upperLeft, lowerRight);
			int sumLumi = 0;
			for (int i = 0; i < subImage.width(); i++) {
				for (int j = 0; j < subImage.height(); j++) {
					sumLumi = sumLumi + subImage[i, j]; //convolve
				}
			}
			
			int temp = 255 - (sumLumi / (step*step));
			//std::cout << temp << "\n";
			subImage = temp;
		}
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
    
    StdArray imageArray(imageInfo.shape());     
	FloatArray imageArrayDest(imageInfo.shape()); 
    // copy image data from file into array
    importImage(imageInfo, imageArray);
	cutImage(imageArray, 700);
	//cannyEdgeImageWithThinning(imageArray, imageArray, 0.8, 4.0, 1, false);
    
	exportImage(imageArray, "face_grey.jpg");
	extractValley(imageArray);
	exportImage(imageArray, "face_edited.jpg");

    return 0;
}