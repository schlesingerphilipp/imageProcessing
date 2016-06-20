#include "Fields.h"
using namespace vigra;

Fields::Fields(MultiArray<2,float> &valley, std::vector<Shape2> &valleyLocals, MultiArray<2,float> &peak, MultiArray<2,float> &edge, MultiArray<2,float> &intensity):
    valleyField(valley), valleys(valleyLocals), peakField(peak), edgeField(edge), intensityField(intensity){};
  Fields::~Fields(){};