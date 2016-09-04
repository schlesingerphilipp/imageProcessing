#ifndef  FIT_TEMPLATE_H
#define  FIT_TEMPLATE_H

#include "../utils/Fields.h"

class FitTemplate
{
    private:
    public:
    int radius;
    Fields fields;

    FitTemplate(Fields & f, int r = 0): radius(r), fields(f){};
    void fit(int valleyWeight = 0, int edgeWeight = 0);
};
#endif 