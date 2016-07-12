#include "fieldAlgorithms.h"
#include <vigra/multi_array.hxx>
#include <vigra/correlation.hxx>
#include <vigra/imagecontainer.hxx>
#include "pyramid.cpp"
#include <vigra/impex.hxx>
#include <iostream>
#include <vigra/multi_convolution.hxx>
#include <vigra/multi_blockwise.hxx>
#include <vigra/convolution.hxx> 
 #include <vigra/multi_morphology.hxx>
#include <vigra/matrix.hxx>
#include <vector>
#include "../utils/Fields.cpp"
#include <stdlib.h>



using namespace vigra;
 using namespace vigra::multi_math;

Fields FieldAlgorithms::fieldsByLaplasian(MultiArray<2, float> & image) 
{
    MultiArray<2, float> valley(image.shape());
    Pyramid pyramid(image);
    for (int i = 3; i > 0; i--)
    {
        MultiArray<2, float> resized(pyramid.get(i));
        MultiArray<2, float> tmpArr(resized.shape());
        laplacianOfGaussianMultiArray(resized, tmpArr, 1.0);
        valley += pyramid.toOriginalSize(tmpArr);
    }
    MultiArray<2, float> peak = valley * -1;
    //remove DC component
    float thrhld = valley[argMax(valley)] * 0.3;
    threshold(valley, valley, thrhld);
    thrhld = peak[argMax(peak)] * 0.3;
    threshold(peak, peak, thrhld);
    

    //Edge as Gradients
    MultiArray<2, float> edgeField(image.shape());
    gaussianGradientMagnitude(image, edgeField, 1.0);

    //Localize
    std::vector<Shape2> valleyLocals(localizePOI(image));

    //Result as Field Object
    Fields fields(valley, valleyLocals, peak, edgeField, image);
    //A heuristic initialization of the localization, as a priori known position
    Shape2 nextToIris = Shape2(image.width() / 2, image.height() / 2);
    fields.specializedIrisValley = localizeByFollowingLocalMaxima(image, nextToIris);
    return fields;
}

Fields FieldAlgorithms::fieldsByGradientPattern(MultiArray<2, float> & image) 
{
    //Get Masks for Valley and Peak
    vigra::ImageImportInfo valleyInfo("../images/valleyMask.png");
    MultiArray<2, float>  valleyArray(valleyInfo.shape());  
    importImage(valleyInfo, valleyArray);
    MultiArray<2, float > valleyMask(9,9);
    resizeImageNoInterpolation(valleyArray, valleyMask);
    vigra::ImageImportInfo peakInfo("../images/peakMask.png");
    MultiArray<2, float>  peakArray(peakInfo.shape());  
    importImage(peakInfo, peakArray);
    MultiArray<2, float > peakMask(9,9);
    resizeImageNoInterpolation(peakArray, peakMask);

    //Actual algorithm for Blobs
    Pyramid pyramid(image);
    MultiArray<2, float> valleyField(image.shape());
    MultiArray<2, float> peakField(image.shape());
    for (int i = 3; i > 0; i--)
    {
        MultiArray<2, float> resized(pyramid.get(i));
        MultiArray<2, float> tmpArr = morphologyByGradientPattern(resized, valleyMask);
        valleyField += pyramid.toOriginalSize(tmpArr);
        tmpArr = morphologyByGradientPattern(resized, peakMask);
        peakField += pyramid.toOriginalSize(tmpArr);
    }

    //Edge as Gradients
    MultiArray<2, float> edgeField(image.shape());
    gaussianGradientMagnitude(image, edgeField, 1.0);

    //Localize
    MultiArray<2, float> smoothed(valleyField.shape());
    gaussianSmoothMultiArray(valleyField, smoothed, 5.0);
    std::vector<Shape2> valleyLocals(localizePOI(smoothed));

    //Result as Field Object
    Fields fields(valleyField, valleyLocals, peakField, edgeField, image);
    //A heuristic initialization of the localization, as a priori known position
    Shape2 nextToIris = Shape2(image.width() / 2 + 10, image.height() / 2);
    fields.specializedIrisValley = Shape2(image.width() / 2 + 10, image.height() / 2 + 15);//localizeByFollowingLocalMaxima(valleyField, nextToIris);
    return fields;
};



Fields FieldAlgorithms::fieldsByErosionDilation(MultiArray<2, float> & image) 
{
    Shape2 shape = image.shape();
    MultiArray<2, float> source(image);
    MultiArray<2, float> t1(shape);
    MultiArray<2, float> t2(shape);

    double radius = 9; //Sinnvoll?

    multiGrayscaleErosion(source, t1, radius);
    multiGrayscaleDilation(source, t2, radius);

    MultiArray<2, float> psi_e = (t1- t2) * -1;
    //Opening of source
    multiGrayscaleErosion(source, t1, radius);
    multiGrayscaleDilation(t1, t2, radius);

    MultiArray<2, float> psi_p = source - t2;
    MultiArray<2, float> psi_pSmooth(psi_p.shape());
    gaussianSmoothMultiArray(psi_p, psi_pSmooth, 8.0);
    //Opening of source
    MultiArray<2, float> inverse = source * -1;
    multiGrayscaleErosion(inverse, t1, 9);
    multiGrayscaleDilation(t1, t2, 9);
    MultiArray<2, float> psi_v = t2 - inverse; 
    MultiArray<2, float> psi_vSmooth(psi_v.shape());
    gaussianSmoothMultiArray(psi_v, psi_vSmooth, 3.0);
    std::vector<Shape2> valleyLocals(0);
    Fields fields(psi_vSmooth, valleyLocals, psi_pSmooth, psi_e, image);
    return fields;
};


std::vector<Shape2> FieldAlgorithms::localizePOI(MultiArray<2, float> &image)
{
    exportImage(image, "./../images/results/wtf.png");
    std::vector<Shape2> pois(1);
    for (int i = 0; i < pois.size(); i++)
    {
        int v1 = std::rand() % image.size() -1; 
         //int maxIndex = argMax(box);
         //Shape2 max(box.scanOrderIndexToCoordinate(maxIndex));
         //image.scanOrderIndexToCoordinate(v1)
        Shape2 poi = localizeByFollowingLocalMaxima(image, Shape2(image.width()/2, image.height()/2));
        pois[i] = poi;
    }
    return pois;

}
Shape2 FieldAlgorithms::localizeByFollowingLocalMaxima(const MultiArray<2, float> &image, Shape2 current)
{

    //Open viewBox of image with center at current
    int upperLeftX = current[0] - ((image.width() / 10) / 2);
    upperLeftX = upperLeftX > -1 ? upperLeftX : 0;
    int upperLeftY = current[1] - ((image.height() / 10) / 2);
    upperLeftY = upperLeftY > -1 ? upperLeftY : 0;
    Shape2 upperLeft(upperLeftX, upperLeftY);
     

    int lowerRightX = current[0] + ((image.width() / 10) / 2);
    lowerRightX = lowerRightX < image.width() ? lowerRightX : image.width() -1;
    int lowerRightY = current[1] + ((image.height() / 10) / 2);
    lowerRightY = lowerRightY < image.height() ? lowerRightY : image.height() -1;
    Shape2 lowerRight(lowerRightX, lowerRightY);
    MultiArrayView<2, float> box = image.subarray(upperLeft, lowerRight);
    //3: get local max of view as next 
    int maxIndex = argMax(box);
    if (maxIndex == -1)
    {
            std::cout << "Something went wrong: argMax returned -1";
        return current;
    }
    Shape2 max(box.scanOrderIndexToCoordinate(maxIndex));
    Shape2 next(upperLeftX + max[0], upperLeftY + max[1]);
    if (next == current)
    {
        return next;
    }
    return localizeByFollowingLocalMaxima(image, next);
}

std::vector<Shape2> FieldAlgorithms::localizePOIExample(const MultiArray<2, float> &image,  MultiArray<2, RGBValue<UInt8> > &rgb_array)
{
    std::vector<Shape2> pois(30);
    float thld = image[argMax(image)] * 0.2;
    for (int i = 0; i < pois.size(); i++)
    {
        int v1 = std::rand() % image.size() -1;         // v1 in the range 0 to image.size()
        Shape2 poi(image.scanOrderIndexToCoordinate(v1));
        poi = localizeByFollowingLocalMaxima(image, poi);
        if (image[poi] > thld)
        {
            pois[i] = poi;
            MultiArrayView<2, RGBValue<UInt8> > markAsStep(rgb_array.subarray(Shape2(poi[0] - 5, poi[1] -5), Shape2(poi[0] +5, poi[1] +5)));
            for (RGBValue<UInt8> & val : markAsStep)
            {
                val.setRed(200);
            }
        }
        else
        {
            i--;
        }
    }
    return pois;

}


bool FieldAlgorithms::contains(std::vector<Shape2> &vector, Shape2 &shape)
{
    for (Shape2 other : vector)
    {
        if (other == shape)
        {
            return true;
        }
    }
    return false;
}

void FieldAlgorithms::threshold(MultiArray<2, float> &basis, MultiArray<2, float> &target, float threshold)
{
    for (int j = 0; j < basis.size(); j++)
    {
        if (basis[j] < threshold)
        {
            target[j] = 0;
        }
    }
};

void FieldAlgorithms::thresholdGrad(MultiArray<2, float> &basis, MultiArray<2, TinyVector<float, 2>> &target, float threshold)
{
    for (int j = 0; j < basis.size(); j++)
    {
        if (basis[j] < threshold)
        {
            target[j][0] = 0;
            target[j][1] = 0;
        }
    }
};


MultiArray<2, float > FieldAlgorithms::matchGradients(MultiArray<2, TinyVector<float, 2> > &imageGradients, MultiArray<2, TinyVector<float, 2> > &mask)
{
    MultiArray<2, float >  dest(imageGradients.shape());
    dest = 0;
    //to make sure, that the mask is always fully within the image, consider the mask shape
    int diff = (mask.width() -1) / 2;
    for (int x = diff; x + diff < imageGradients.width(); x ++) 
    {
        for (int y = diff; y + diff < imageGradients.height(); y++)
        {
            //The masks center is at point x,y now
            //Every vector of the image currently 'covered' by the mask, is compared to its corresponding vector in the mask.
            float vals = 0;
            for (int xM = 0; xM < mask.width(); xM++)
            {
                for (int yM = 0; yM < mask.height(); yM++)
                {
                    TinyVector<float, 2> imageVal = imageGradients((x - diff) + xM, (y - diff) + yM);
                    TinyVector<float, 2> maskVal = mask(xM, yM);
                    vals += compareVectors(imageVal, maskVal);
                }
            }
            int res = vals / (mask.size());
            dest(x,y) = res;
        }
    }
    return dest;
};

int FieldAlgorithms::compareVectors(TinyVector<float, 2> &compared, TinyVector<float, 2> &expected) 
{
    int val = 0;
    int xC = compared[0];
    int yC = compared[1];
    int xE = expected[0];
    int yE = expected[1];
    if ((xC > 0 && xE > 0) || (xC < 0 && xE < 0))
    {
        val++;
    }
    if ((yC > 0 && yE > 0) || (yC < 0 && yE < 0))
    {
        val++;
    }
    if ((xC > yC && xE > yE) || (xC < yC && xE < yE))
    {
        val++;
    }
    return val;
}

MultiArray<2, float > FieldAlgorithms::morphologyByGradientPattern(MultiArray<2, float> & image, MultiArray<2,float> &mask) {
    
    Shape2 shape(image.shape());
    MultiArray<2, TinyVector<float, 2>> imageGradients(shape);
    MultiArray<2, TinyVector<float, 2>> maskGradients(mask.shape());
    gaussianGradientMultiArray(image, imageGradients, 2.0);
    gaussianGradientMultiArray(mask, maskGradients, 2.0);

    //Threshold gradients with very small magnitude, 
    //is a kind of smoothing noise, since we only consider directions from now on.
    MultiArray<2, float> magnitudes(shape);
    gaussianGradientMagnitude(image, magnitudes, 3.0);
    float thrhld = magnitudes[argMax(magnitudes)] * 0.3;
    thresholdGrad(magnitudes, imageGradients, thrhld);
    //The actual machting:
    MultiArray<2, float > gradientMatch = matchGradients(imageGradients, maskGradients);
   
    return gradientMatch;
};

  MultiArray<2,int> FieldAlgorithms::binarizeByThreshold(MultiArray<2,float> &basis, float boarder)
  {
          float thrhld = basis[argMax(basis)] * boarder;
          MultiArray<2,float> throlded(basis);
          threshold(basis, throlded, thrhld);
          MultiArray<2,int> binary(basis.shape());
          for (int i = 0; i < throlded.size(); i++)
          {
              if (throlded[i] > 0)
              {
                  binary[i] = 1;
              }
          }
          return binary;
  }
