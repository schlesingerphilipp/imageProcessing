#ifndef  VALLEY_LOCALIZATION_H
#define  VALLEY_LOCALIZATION_H
#include "../utils/Point.h"
#include <vigra/multi_array.hxx>
#include <vigra/correlation.hxx>


using namespace vigra;

class ValleyLocalization {
  private:
  public:
  ValleyLocalization();
  Point localize(MultiArray<2, float> & valley); 
  MultiArray<2, float > valley(MultiArray<2, float > & image); 
};
#endif 