#ifndef Curve_hpp
#define Curve_hpp

#include <stdio.h>
#include <vigra/diff2d.hxx>

using namespace vigra;

class Curve {
public:
	Curve(Point2D c1, Point2D c2, Point2D c3):
	c1(c1),
	c2(c2),
	c3(c3) {}
	
	std::string to_string();
	
	Point2D c1;
	Point2D c2;
	Point2D c3;
};

#endif /* Curve_hpp */
