#include "deformation.h"
#include "../utils/RadiusResult.h"
#include <stdlib.h> 
#include <cmath>
#include <vigra/impex.hxx>
#include <iostream>
 using namespace vigra;
 using namespace vigra::multi_math;

int Deformation::scaleByRadius(Shape2 &local) 
{
    //Build a representation of the distance for each pixel to some Shape2(x,y) position
    MultiArray<2, int> distanceToCenter(fields.edgeField.shape());
    for (int iX = 0; iX < distanceToCenter.width(); iX++)
    {
        for (int iY = 0; iY < distanceToCenter.height(); iY++)
        {           
            int distance = std::sqrt(pow((local[0] - iX),2) + std::pow((local[1] - iY),2));
            distanceToCenter(iX, iY) = distance;
        }
    }

    //arbitrary initialization 'start from small'
    RadiusResult two = getValueForRadius(distanceToCenter, 1);
    RadiusResult one = getValueForRadius(distanceToCenter, 2 );
    RadiusResult rr = getRadiusRecursivly(distanceToCenter, one, two, 10.0);

    //uncomment to get images of the three functions of radius and fit: How well does the area, the edge of the radius fit?
    //drawFunctions(distanceToCenter);

    //uncomment to get a nice result picture, where a circle with the fitted radius is drawn
    for (int i = 0; i < distanceToCenter.size();i++)
    {
        for (int lineWidth = 0; lineWidth < 4; lineWidth++)
        {
            if (distanceToCenter[i] == rr.radius + lineWidth || distanceToCenter[i] == rr.radius - lineWidth)
            {
                fields.intensityField[i] = 0;
            }
        }
    }
    return rr.radius; 
};

RadiusResult Deformation::getRadiusRecursivly(MultiArray<2, int> &distanceToCenter, RadiusResult &one, RadiusResult &two, float temperature)
{
    if (temperature < 1)
    {
        return one;
    }
    if (one.radius == two.radius)
    {
        one.radius++;
    }
    float derivative = (one.getValue() - two.getValue()) / (one.radius - two.radius);
    derivative = derivative < 0 && derivative > -1 ? -1 : derivative > 0 && derivative < 1 ? 1 : derivative;
    int nextRadius = (one.radius + (derivative * temperature)); //TODO: rethink temperature and check for local maxima
    //prevent values larger than the image
    nextRadius = nextRadius < 0 ? nextRadius * -1 : nextRadius;
    nextRadius = nextRadius % (distanceToCenter.width() / 2);

    RadiusResult next = getValueForRadius(distanceToCenter, nextRadius);
    return getRadiusRecursivly(distanceToCenter, next, one, temperature * 0.95);
};

RadiusResult Deformation::getValueForRadius(MultiArray<2, int> &distanceToCenter, int radius)
{
    float valley = 0;
    int valleyCount = 0;
    float edge = 0;
    int edgeCount = 0;
    for (int i = 0; i < fields.edgeField.size(); i++)
    {

        if (distanceToCenter[i] < radius) 
        {
            valley += fields.valleyField[i];
            valleyCount++;
        }
        if (distanceToCenter[i] == radius)
        {
            edge += fields.edgeField[i];
            edgeCount++;
        }
    }

    float valleys = valleyCount > 0 ? valley / valleyCount : 0;
    float edges = edgeCount > 0 ? edge / edgeCount : 0;
    RadiusResult res(radius, valleys, edges, 1);
    return res;
};

void Deformation::drawFunctions(MultiArray<2, int> &distanceToCenter)
{
    std::vector<MultiArray<2,float>> functions = getFit(distanceToCenter);
    MultiArray<2,float> resEdge(distanceToCenter.shape());
    MultiArray<2,float> resValley(distanceToCenter.shape());
    MultiArray<2,float> resBoth(distanceToCenter.shape());
    for (int i = 0; i<distanceToCenter.width() / 2;i++)
    {
        int yEdge = functions[0][i] > distanceToCenter.height() ? distanceToCenter.height() -1 : (distanceToCenter.height() - (functions[0][i] -1));
        int yValley = functions[1][i] > distanceToCenter.height() ? distanceToCenter.height() -1 : (distanceToCenter.height() - (functions[1][i] -1));
        int yBoth = functions[2][i] > distanceToCenter.height() ? distanceToCenter.height() -1 : (distanceToCenter.height() - (functions[2][i] -1));
        resEdge(i, yEdge) = 1;
        resValley(i, yValley) = 1;
        resBoth(i, yBoth) = 1;
    }
    exportImage(resEdge,"./../images/results/edgeFunction.png");
    exportImage(resValley,"./../images/results/valleyFunction.png");
    exportImage(resBoth ,"./../images/results/bothFunction.png");
    std::cout << "\n expected Radius: ";
    int rad = argMax(functions[2]);
    std::cout << rad;
    functions[2][rad] = 0;
    rad = argMax(functions[2]);
    std::cout << "\n next best Radius: ";
    std::cout << rad;
    functions[2][rad] = 0;
    rad = argMax(functions[2]);
    std::cout << "\n next best Radius: ";
    std::cout << rad;
    functions[2][rad] = 0;
    rad = argMax(functions[2]);
    std::cout << "\n next best Radius: ";
    std::cout << rad;
    functions[2][rad] = 0;
    rad = argMax(functions[2]);
    std::cout << "\n";
    
}

//Calculates the fit for all radii
std::vector<MultiArray<2,float>> Deformation::getFit(MultiArray<2, int> &distanceToCenter) 
{
    int length = distanceToCenter.width();
    MultiArray<2,float> resEdge(Shape2(length, 1));
    MultiArray<2,float> resValley(Shape2(length, 1));
    MultiArray<2,float> resBoth(Shape2(length, 1));
    for (int radius = 1; radius < distanceToCenter.width() / 2; radius++)
    {
        RadiusResult radi = getValueForRadius(distanceToCenter, radius);
        resValley[radius] = radi.valley;
        resBoth[radius] = radi.getValue();
        resEdge[radius] = radi.edge;
    }
    std::vector<MultiArray<2,float>>result(3);
    result[0] = resEdge;
    result[1] = resValley;
    result[2] = resBoth;
    return result;
}