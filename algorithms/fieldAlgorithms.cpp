#include "fieldAlgorithms.h"
#include <vigra/multi_array.hxx>
#include <vigra/correlation.hxx>
#include "../utils/Point.h"
#include "../utils/Point.cpp"
#include <vigra/imagecontainer.hxx>
#include "pyramid.cpp"
#include <vigra/impex.hxx>
#include <iostream>




using namespace vigra;

MultiArray<2, float > FieldAlgorithms::valley(MultiArray<2, float> & image) {
    MultiArray<2, float > valley(image.shape());
    valley = 0;
    
    Pyramid pyramid(image);
    MultiArray<2, float >  mask = MultiArray<2, float >(5,5);
    mask = 50;
    mask(2,1) = 0;
    mask(2,2) = 0;
    mask(2,3) = 0;
    mask(1,2) = 0;
    mask(3,2) = 0;
    for (int i = 5; i > 0; i--)
    {
        MultiArray<2, float> resized(pyramid.get(i));
        MultiArray<2, float> tmpArr(Shape2(resized.width(), resized.height()));
        fastNormalizedCrossCorrelation(resized, mask, tmpArr);
        for (float val : tmpArr) 
        {
            if (val > 0.7)
            {
                std::cout << "YES!\n";
                tmpArr = deNormalize(tmpArr, 0.5);
                valley = pyramid.toOriginalSize(tmpArr);
                resized = pyramid.toOriginalSize(resized);
                exportImage(resized, "../images/resized.png");
                return valley;
            }
        }
    }
    std::cout << "on main image!\n";
    fastNormalizedCrossCorrelation(image, mask, valley);
    valley = deNormalize(valley, 0.5);
    return valley;
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
MultiArray<2, float> FieldAlgorithms::deNormalize(MultiArray<2, float> normalized, float threshold)
{
    for (int j = 0; j < normalized.size(); j++)
    {
        if (normalized[j] > threshold)
        {
            normalized[j] = normalized[j] * 255;
        }
    }
    return normalized;
};
