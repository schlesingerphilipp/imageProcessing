#ifndef  DEFORMATION_H
#define  DEFORMATION_H
#include "../utils/Fields.h"
#include "../utils/RadiusResult.h"
#include <vigra/impex.hxx>



using namespace vigra;

class Deformation {
    private:
        
        RadiusResult getRadiusRecursivly(MultiArray<2, int> &distanceToCenter, RadiusResult &one, RadiusResult &two, float temperature);
        RadiusResult getValueForRadius(MultiArray<2, int> &distanceToCenter, int radius);
        std::vector<MultiArray<2,float>> getFit(MultiArray<2, int> &distanceToCenter); 
        void drawFunctions(MultiArray<2, int> &distanceToCenter); 
    public:
        Fields fields;
        Deformation(Fields &f):fields(f){};
        int scaleByRadius(Shape2 &local);

};
#endif 