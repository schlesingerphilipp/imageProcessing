

#ifndef Circle_hpp
#define Circle_hpp

#include <stdio.h>
#include "Curve.hpp"

using namespace vigra;

class Circle {
public:
	Circle(Point2D centre, Curve curve1, Curve curve2, Curve curve3, Curve curve4):
	centre(centre),
	curve1(curve1),
	curve2(curve2),
	curve3(curve3),
	curve4(curve4) {}
	
	std::string to_string();
	
	Point2D centre;
	Curve curve1;
	Curve curve2;
	Curve curve3;
	Curve curve4;
};

#endif /* Circle_hpp */
