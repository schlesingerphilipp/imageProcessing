#ifndef  VALLEY_LOCALIZATION_H
#define  VALLEY_LOCALIZATION_H
#include "../utils/Point.h"
#include <vigra/multi_array.hxx>
#include <vigra/correlation.hxx>
#include <vector>


using namespace vigra;

class FieldAlgorithms {
  private:
  static MultiArray<2, float> deNormalize(MultiArray<2,float> normalized, float threshold);
  static MultiArray<2, float> asOctaveGradientAngle(MultiArray<2,float> image);
  static MultiArray<2,float> meanSqurError(MultiArray<2, TinyVector<float, 2> > image, MultiArray<2, TinyVector<float, 2> > mask);
  public:
  static std::vector<MultiArray<2, float>> fieldsByErosionDilation(MultiArray<2, float> & image);
  static Point localize(MultiArray<2, float> & valley); 
  static MultiArray<2, float > valley(MultiArray<2, float >  & image); 
};
#endif 