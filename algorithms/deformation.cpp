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
    
    MultiArray<2, int> distanceToCenter(fields.edgeField.shape());
    for (int iX = 0; iX < distanceToCenter.width(); iX++)
    {
        for (int iY = 0; iY < distanceToCenter.height(); iY++)
        {           
            int distance = std::sqrt(pow((local[0] - iX),2) + std::pow((local[1] - iY),2));
            distanceToCenter(iX, iY) = distance;
        }
    }
    //drawFunctions(distanceToCenter);
    RadiusResult two = getValueForRadius(distanceToCenter, 1);
    RadiusResult one = getValueForRadius(distanceToCenter, 2 );
    /*MultiArray<2,int> plotTemperature(distanceToCenter.shape());
    for(int i = 0; i < plotTemperature.width() / 2; i++)
    {
        int y = getRadiusRecursivly(distanceToCenter, one, two, i).radius;
        plotTemperature(i, y) = 1;
    }
    exportImage(plotTemperature,"./../images/results/plotTemperature.png"); */
    RadiusResult rr = getRadiusRecursivly(distanceToCenter, one, two, 10.0);
    //rr.radius = 10;
    //draw a circle with the radius at the localization
    for (int i = 0; i < distanceToCenter.size();i++)
    {
        if (distanceToCenter[i] == rr.radius)
        {
            fields.intensityField[i] = 255;
        }
        if (distanceToCenter[i] == rr.radius +1)
        {
            fields.intensityField[i] = 255;
        }
        if (distanceToCenter[i] == rr.radius +2)
        {
            fields.intensityField[i] = 255;
        }
        if (distanceToCenter[i] == rr.radius +3)
        {
            fields.intensityField[i] = 255;
        }
        if (distanceToCenter[i] == rr.radius -1)
        {
            fields.intensityField[i] = 255;
        }
        if (distanceToCenter[i] == rr.radius -2)
        {
            fields.intensityField[i] = 255;
        }
        if (distanceToCenter[i] == rr.radius -3)
        {
            fields.intensityField[i] = 255;
        }
    }
    return rr.radius; 
};

RadiusResult Deformation::getRadiusRecursivly(MultiArray<2, int> &distanceToCenter, RadiusResult &one, RadiusResult &two, float temperature)
{
    //std::cout << ".";
    /*std::cout << "\n 1 radius:";
    std::cout << one.radius;
    std::cout << "\n 1 val:";
    std::cout << one.getValue();
    std::cout << "\n 2 radius:";
    std::cout << two.radius;
    std::cout << "\n 2 val:";
    std::cout << two.getValue();
    std::cout << "\n";
    */
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
    //std::cout << derivative;
    int nextRadius = (one.radius + (derivative * temperature)); //one.radius + derivative;//
        
    nextRadius = nextRadius < 0 ? nextRadius * -1 : nextRadius;
    //std::cout << nextRadius;
    nextRadius = nextRadius % (distanceToCenter.width() / 2);
    //nextRadius = nextRadius < 0 ? distanceToCenter.width() / 2 : nextRadius > distanceToCenter.width() / 2 ? 1 : nextRadius;
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
    /*std::cout << "\n expected Radius: ";
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
    */
}