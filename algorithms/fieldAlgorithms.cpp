#include "fieldAlgorithms.h"
#include <vigra/multi_array.hxx>
#include <vigra/correlation.hxx>
#include "../utils/Point.h"
#include "../utils/Point.cpp"
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



using namespace vigra;
 using namespace vigra::multi_math;

MultiArray<2, float > FieldAlgorithms::valley(MultiArray<2, float> & image) {
    MultiArray<2, float > valley(image.shape());
    valley = 0;
    MultiArray<2, float > mask(3,3);
    mask = 250;
    mask(1,1) = 150;

    //resizeImageNoInterpolation(protoMask, mask);
    MultiArray<2, float> gradients(image.shape());
    MultiArray<2, float> smoothed(image.shape());
    gaussianSmoothMultiArray(image, smoothed, 3.0);
    gaussianGradientMagnitude(smoothed,gradients,1.0);
    //gaussianSmoothMultiArray(gradients, image, 8.0);
    Pyramid pyramid(image);
    for (int i = 4; i > 0; i--)
    {
        //MultiArray<2, float> imageGradientMagnitudes(pyramid.get(i).shape());
        MultiArray<2, float> resized(pyramid.get(i));
        //MultiArray<2, float> resizedG(pyramid.get(i));
        //gaussianSmoothMultiArray(resized, resizedG, 2.0);
        //gaussianGradientMagnitude(resizedG,imageGradientMagnitudes,1.0);
        MultiArray<2, float> tmpArr(Shape2(resized.width(), resized.height()));
        fastNormalizedCrossCorrelation(resized, mask, tmpArr);
        //for (float val : tmpArr) 
        //{
            //if (val > 0.1)
            //{
                std::cout << "YES!\n";
                tmpArr = deNormalize(tmpArr, 0.4);
                valley += pyramid.toOriginalSize(tmpArr);
                //resized = pyramid.toOriginalSize(resized);
              //  exportImage(resized, "../images/resized.png");
                //return valley;
            //}
        //}
    }
//    fastNormalizedCrossCorrelation(image, mask, valley);
 //   valley = deNormalize(valley, 0.5);

    gaussianSmoothMultiArray(image, smoothed, 3.0);
    gaussianGradientMagnitude(smoothed,gradients,1.0);
    gaussianSmoothMultiArray(gradients, valley, 8.0);
    return valley;
};
MultiArray<2, float> FieldAlgorithms::deNormalize(MultiArray<2, float> normalized, float threshold)
{
    for (int j = 0; j < normalized.size(); j++)
    {
        if (normalized[j] > threshold)
        {
            normalized[j] = normalized[j] * 255;
        }
        else 
        {
            normalized[j] = 0;
        }
    }
    return normalized;
};
  
MultiArray<2, float > FieldAlgorithms::asOctaveGradientAngle(MultiArray<2, float >  image) 
{
    MultiArray<2, TinyVector<float, 2> > gradients(image.shape());
    gaussianGradient(image,gradients,1.0);

    for (int i = 0; i < gradients.size(); i++)
    {
        TinyVector<float, 2> gradient = gradients[i];
        //Set very small values to 0;
        if (gradient[0] < 0.1 && gradient[0] > -0.1) {
            gradient[0] = 0;
        }
        if (gradient[1] < 0.1 && gradient[1] > -0.1) {
            gradient[1] = 0;
        }
        float angle; //variing between 1/8 and 8/8 for all neighbouring pixels, or 0.
        angle = (gradient[0] == 0 && gradient[1] == 0) ? 0 :
                (gradient[0] == 0 && gradient[1] > 0) ? 1/8 :
                (gradient[0] > 0 && gradient[1] > 0) ? 2/8 :
                (gradient[0] > 0 && gradient[1] == 0) ? 3/8 :
                (gradient[0] > 0 && gradient[1] < 0) ? 4/8 :
                (gradient[0] == 0 && gradient[1] < 0) ? 5/8 :
                (gradient[0] < 0 && gradient[1] < 0) ? 6/8 :
                (gradient[0] < 0 && gradient[1] == 0) ? 7/8 :
                //(gradient[0] < 0 && gradient[1] > 0) ? -> 8/8
                8/8;
                image[i] = angle;
    }
    return image;
};

Point FieldAlgorithms::localize(MultiArray<2, float> & valley)
  {
      float max = 0;
      Point p(0,0);
      for (int x = 0; x < valley.shape(0); x++)
      {
          for (int y = 0; y < valley.shape(1); y++)
          {
              float current = valley(x,y);
              if (current > max)
              {
                  max = current;
                  p = Point(x,y);
              }
          }
      }
      return p;
  }; 

std::vector<MultiArray<2, float>> FieldAlgorithms::fieldsByErosionDilation(MultiArray<2, float> & image) 
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

    MultiArray<2, float> psi_v = t2 - source;
    MultiArray<2, float> psi_vSmooth(psi_v.shape());
    gaussianSmoothMultiArray(psi_v, psi_vSmooth, 8.0);
    std::vector<MultiArray<2, float>> fields(3);
    fields[0] = psi_vSmooth;
    fields[1] = psi_pSmooth;
    fields[2] = psi_e;
    return fields;
};