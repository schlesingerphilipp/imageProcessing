#include "Curve.hpp"

std::string Curve::to_string() {
	std::string c1 = std::to_string(this->c1.px()) + "," + std::to_string(this->c1.py());
	std::string c2 = std::to_string(this->c2.px()) + "," + std::to_string(this->c2.py());
	std::string c3 = std::to_string(this->c3.px()) + "," + std::to_string(this->c3.py());
	
	return (c1 + " " + c2 + " " + c3);
}