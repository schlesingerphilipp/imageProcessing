#ifndef  VALLEY_LOCALIZATION_H
#define  VALLEY_LOCALIZATION_H
#include "../utils/Fields.h"
#include <vigra/multi_array.hxx>
#include <vigra/correlation.hxx>
#include <vector>


using namespace vigra;

class FieldAlgorithms {
  private:
  static void threshold(MultiArray<2,float> &basis, MultiArray<2,float> &target, float threshold);
  static void thresholdGrad(MultiArray<2,float> &basis, MultiArray<2,TinyVector<float, 2> > &target, float threshold);
  static MultiArray<2, float > morphologyByGradientPattern(MultiArray<2,float> &image, MultiArray<2,float> &pattern);
  static MultiArray<2, float >  matchGradients(MultiArray<2, TinyVector<float, 2>> & image, MultiArray<2,TinyVector<float, 2>> &mask);
  static int compareVectors(TinyVector<float, 2> &compared, TinyVector<float, 2> &comperator);
  static MultiArray<2,int> binarizeByThreshold(MultiArray<2,float> &basis, float threshold);
  static std::vector<Shape2> localizePOI(MultiArray<2, float> &image);
  static Shape2 localizeByFollowingLocalMaxima(const MultiArray<2, float> &vieBox, Shape2 current);
  static bool contains(std::vector<Shape2> &vec, Shape2 &other);

  public:
    static std::vector<Shape2> localizePOIExample(const MultiArray<2, float> &image, MultiArray<2, RGBValue<UInt8> > &rgb_array);

  static Fields fieldsByErosionDilation(MultiArray<2, float> & image);
  static Fields fieldsByGradientPattern(MultiArray<2, float> & image);
  static Fields fieldsByLaplasian(MultiArray<2, float> & image); 
};
#endif 