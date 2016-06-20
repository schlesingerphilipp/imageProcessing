#ifndef  FIELDS_H
#define  FIELDS_H

#include <vector>
#include <vigra/multi_array.hxx>

using namespace vigra;

class Fields {
  private:
  public:
  Fields(MultiArray<2,float> &valley, std::vector<Shape2> &valleyLocals,  MultiArray<2,float> &peak, MultiArray<2,float> &edge, MultiArray<2,float> &intensity);
  ~Fields();
  const MultiArray<2,float> valleyField;
  const std::vector<Shape2> valleys;
  const MultiArray<2,float> peakField;
  const MultiArray<2,float> edgeField;
  const MultiArray<2,float> intensityField;
};
#endif 