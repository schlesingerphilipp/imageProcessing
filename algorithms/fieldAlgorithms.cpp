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
    MultiArray<2, float> edgeField(image.shape());
    MultiArray<2, float> peak = valley * -1;
    float thrhld = valley[argMax(valley)] * 0.3;
    threshold(valley, valley, thrhld);
    thrhld = peak[argMax(peak)] * 0.3;
    threshold(peak, peak, thrhld);
    std::vector<Shape2> valleyLocals(0);
    Fields fields(valley, valleyLocals, peak, edgeField, image);
    return fields;
}

Fields FieldAlgorithms::fieldsByGradientPattern(MultiArray<2, float> & image) 
{
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
    MultiArray<2, float> edgeField(image.shape());
    std::vector<Shape2> valleyLocals(0);
    Fields fields(valleyField, valleyLocals, peakField, edgeField, image);
    return fields;
};

Fields FieldAlgorithms::fieldsByErosionDilation(MultiArray<2, float> & image) 
{
    Shape2 shape = image.shape();
    MultiArray<2, float> source(image);
    MultiArray<2, float> t1(shape);
    MultiArray<2, float> t2(shape);

    double radius = 2; //Sinnvoll?

    multiGrayscaleErosion(source, t1, radius);
    multiGrayscaleDilation(source, t2, radius);

    MultiArray<2, float> psi_e = (t1- t2) * -1;
    //Opening of source
    multiGrayscaleErosion(source, t1, radius);
    multiGrayscaleDilation(t1, t2, radius);

    MultiArray<2, float> psi_p = source - t2;
    MultiArray<2, float> psi_pSmooth(psi_p.shape());
    gaussianSmoothMultiArray(psi_p, psi_pSmooth, 8.0);
    //Closing of source
    multiGrayscaleDilation(source, t1, radius);
    multiGrayscaleErosion(t1, t2, radius);

    MultiArray<2, float> magnitudes(shape);
    gaussianGradientMagnitude(image, magnitudes, 3.0);
    MultiArray<2, float> dialtion(shape);
    MultiArray<2, float> erosion(shape);
    MultiArray<2, int> binary(binarizeByThreshold(magnitudes, 0.3));
    multiBinaryDilation(binary, dialtion, 5.0);
    multiBinaryErosion(dialtion, erosion, 5.0);
    MultiArray<2, float> psi_v = erosion;
    MultiArray<2, float> psi_vSmooth(psi_v.shape());
    gaussianSmoothMultiArray(psi_v, psi_vSmooth, 8.0);
    std::vector<Shape2> valleyLocals(0);

    MultiArray<2, float> psi_v2 = t2 - source; 
    Fields fields(psi_v2, valleyLocals, psi_pSmooth, psi_e, image);
    return fields;
};


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
            //A perfect fit would be: vals / mask.size() = 1, but is unlikely, so be tolerant.
            int res = vals / (mask.size());// * 0.15);
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
    //To enlarge the distance of interaction smooth with large variance
    MultiArray<2, float> smoothed(shape);
    gaussianSmoothMultiArray(gradientMatch, smoothed, 1.0);
    return smoothed;
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
