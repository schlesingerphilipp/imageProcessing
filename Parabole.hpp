
#ifndef Parabole_hpp
#define Parabole_hpp

#include <stdio.h>
#include <vigra/diff2d.hxx>
#include "Curve.hpp"

using namespace vigra;

class Parabole {
public:
	Parabole(Point2D p1, Curve curve1, Curve curve2, Curve curve3):
	p1(p1),
	curve1(curve1),
	curve2(curve2),
	curve3(curve3) {}
	
	std::string to_string();
	
	Point2D p1;	
	Curve curve1;
	Curve curve2;
	Curve curve3;
};

#endif /* Parabole_hpp */
