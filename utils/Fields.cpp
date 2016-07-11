#include "Fields.h"
using namespace vigra;

Fields::Fields(MultiArray<2,float> &valley, std::vector<Shape2> &valleyLocals, MultiArray<2,float> &peak, MultiArray<2,float> &edge, MultiArray<2,float> &intensity):
    valleyField(valley), valleys(valleyLocals), peakField(peak), edgeField(edge), intensityField(intensity)
    {
      float valley255Factor = 255 / valleyField[argMax(valleyField)];
      float peak255Factor = 255 / peakField[argMax(peakField)];
      float edge255Factor = 255 / edgeField[argMax(edgeField)];
      float intensity255Factor = 255 / intensityField[argMax(intensityField)];
      for (int i = 0; i < intensityField.size(); i++)
      {
        valleyField[i] = valleyField[i] * valley255Factor;
        peakField[i] = peakField[i] * peak255Factor;
        edgeField[i] = edgeField[i] * edge255Factor;
        intensityField[i] = intensityField[i] * intensity255Factor;
      }
    };
  Fields::~Fields(){};