#ifndef  RADIUS_RESULT_H
#define  RADIUS_RESULT_H
class RadiusResult
{
    public:
    int radius;
    float valley;
    float edge;
    float valleyEdgeRatioWeight;
    RadiusResult(int r = 0, float v = 0, float e = 0, float verw = 1): radius(r), valley(v), edge(e), valleyEdgeRatioWeight(verw){};
    float getValue() {return valley + edge / valleyEdgeRatioWeight;};
};
#endif 