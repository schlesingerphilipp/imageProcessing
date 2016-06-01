#include "valleyLocalization.h"
#include <vigra/multi_array.hxx>
#include <vigra/correlation.hxx>
#include "../utils/Point.h"
#include "../utils/Point.cpp"
#include <vigra/imagecontainer.hxx>

using namespace vigra;

ValleyLocalization::ValleyLocalization(){};
MultiArray<2, float > ValleyLocalization::valley(MultiArray<2, float > & image) {
    MultiArray<2, float > valley(image.shape());
    MultiArray<2, float >  mask = MultiArray<2, float >(3,3);
    mask = 0;
    mask(1,1) = 254;
    fastCrossCorrelation(image, mask, valley);
    for (float val : valley) 
    {
        val = val * 254;
    }
    return valley;
    
};
  Point ValleyLocalization::localize(MultiArray<2, float> & valley)
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
