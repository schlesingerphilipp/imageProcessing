
#include "FitTemplate.h"
#include "../utils/Fields.h"
#include "../algorithms/deformation.h"

void FitTemplate::fit(int valleyWeight, int edgeWeight) {
    Deformation deformer(fields);
    radius = deformer.run(valleyWeight, edgeWeight);
};