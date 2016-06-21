#ifndef Template_h
#define Template_h

#include <vigra/diff2d.hxx>
#include "Circle.hpp"
#include "Parabole.hpp"

using namespace vigra;

class Template {
public:
	Template(Circle c, Parabole pUpper, Parabole pLower):
	c(c),
	pUpper(pUpper),
	pLower(pLower) {}
	
	Circle c;
	Parabole pUpper;
	Parabole pLower;
};



#endif /* Template_h */
