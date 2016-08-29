
#include "Parabole.hpp"

std::string Parabole::to_string() {
	std::string start = std::to_string(this->p1.px()) + "," + std::to_string(this->p1.py());
	
	return ("m " + start + " c " + this->curve1.to_string() + " " + this->curve2.to_string()+ " " 
			+ this->curve3.to_string() + "\0");
}