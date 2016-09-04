#ifndef  EPOCH_H
#define  EPOCH_H
#include "FitTemplate.h"

class Epoch
{
    public:
    static void run(FitTemplate & templ, int valleyWeight = 1, bool valleyFixedToDo = true, int edgeWeight = 1, bool edgeFixedToDo = true) {
    templ.fit(valleyWeight, edgeWeight);
    }
};
#endif 