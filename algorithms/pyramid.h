#ifndef  PYRAMID_H
#define  PYRAMID_H
#include <vigra/multi_array.hxx>


using namespace vigra;

class Pyramid {
  private:
  int origHeight, origWidth;
  MultiArray<2, float > original;
  public:
  Pyramid(MultiArray<2, float > original);
  MultiArray<2, float > get(int level);
  MultiArray<2, float > toOriginalSize(MultiArray<2, float > resized); 
};
#endif 