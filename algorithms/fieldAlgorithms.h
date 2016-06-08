#ifndef  VALLEY_LOCALIZATION_H
#define  VALLEY_LOCALIZATION_H
#include "../utils/Point.h"
#include <vigra/multi_array.hxx>
#include <vigra/correlation.hxx>


using namespace vigra;

class FieldAlgorithms {
  private:
  static MultiArray<2, float> deNormalize(MultiArray<2,float> normalized, float threshold);
  public:
  static Point localize(MultiArray<2, float> & valley); 
  static MultiArray<2, float > valley(MultiArray<2, float >  & image); 
};
#endif 