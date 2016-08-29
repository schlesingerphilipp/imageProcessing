
#include "Circle.hpp"
std::string Circle::to_string() {
	std::string centre = std::to_string(this->centre.px()) + "," + std::to_string(this->centre.py());
	
	return ("m " + centre + " c " + this->curve1.to_string() + " " + this->curve2.to_string()+ " " 
			+ this->curve3.to_string()+ " " + this->curve4.to_string() + "\0");
	
}