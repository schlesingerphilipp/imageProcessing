#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "rapidxml-1.13/rapidxml_utils.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"
#include "rapidxml-1.13/rapidxml_iterators.hpp"
#include "rapidxml-1.13/rapidxml.hpp"
#include <vigra/diff2d.hxx>
#include <boost/algorithm/string.hpp>

#include "Template.hpp"

using namespace rapidxml;
using namespace boost;
typedef std::vector<std::string> SplitVec;
typedef rapidxml::xml_node<>* XmlNode;

class svgHandler {

public:
char* loadSVG(char* filename) {
	std::ifstream fstr;
	int length;
	fstr.open(filename);      
	fstr.seekg(0, std::ios::end);    
	length = fstr.tellg();          
	fstr.seekg(0, std::ios::beg);    
	char *buffer = new char[length];   
	fstr.read(buffer, length);       
	fstr.close();
	
	return buffer;
}

private:
SplitVec getData(XmlNode node) {
	
	xml_attribute<> *attr = node->first_attribute("d");	
	std::string attr_value(attr->value());	
	SplitVec splitVec; 
	split(splitVec, attr_value, is_any_of(" "), token_compress_on); 
	
	return splitVec;
}

void setData(xml_document<> doc, XmlNode node, std::string data) {
	const char * newValue = doc.allocate_string(data.c_str(), strlen(data.c_str()));
	xml_attribute<> *attr = node->first_attribute("d");	
	
	attr->value(newValue);
	
}

Point2D getPoint(std::string str) {
	SplitVec p;
	split(p, str, is_any_of(","), token_compress_on);
	const char * pX = p[0].c_str();
	const char * pY = p[1].c_str();
	
	//std::cout << pX << " " << pY << "\n";
	
	return *new Point2D(strtof(pX, NULL), strtof(pY, NULL));
	
}

std::vector<Curve *> getCurves(SplitVec data, int i) {
	std::vector<Curve *> curveVector;
	
	while (i < data.size()) {
		
		Curve *curve = 
		new Curve(getPoint(data[i]), getPoint(data[i + 1]), getPoint(data[i + 2]));
		curveVector.insert(curveVector.end(), curve);
		i = i + 3;
	}
	return curveVector;
}

public:
Template parseSVG(char* fileString) {
	
	xml_document<> doc; 
	doc.parse<0>(fileString);
	
	XmlNode pathNode1 = doc.first_node()->first_node("g")->first_node();
	XmlNode pathNode2 = pathNode1->next_sibling();
	XmlNode pathNode3 = pathNode2->next_sibling();
	
	SplitVec data1 = getData(pathNode1); //circle
	SplitVec data2 = getData(pathNode2); //parabole upper
	SplitVec data3 = getData(pathNode3); //parabole lower
	
	Point2D m(0,0);
	std::vector<Curve *> curveVector;
	int i = 0;
	while (i < data1.size()) {
		if (data1[i] == "m") {
			m = getPoint(data1[i + 1]);
		} else if (data1[i] == "c") {
			curveVector = getCurves(data1, i + 1);
		}
		i++;
	}
	Circle *circle = new Circle(m, *curveVector[0], *curveVector[1], *curveVector[2], *curveVector[3]);
	curveVector.clear();
	
	i = 0;
	while (i < data2.size()) {
		if (data2[i] == "m") {
			m = getPoint(data2[i + 1]);
		} else if (data2[i] == "c") {
			curveVector = getCurves(data2, i + 1);
		}
		i++;
	}
	Parabole *pUpper = new Parabole(m, *curveVector[0], *curveVector[1], *curveVector[2]);	
	
	curveVector.clear();
	
	i = 0;
	while (i < data3.size()) {
		if (data3[i] == "m") {
			m = getPoint(data3[i + 1]);
		} else if (data3[i] == "c") {
			curveVector = getCurves(data3, i + 1);
		}
		i++;
	}
	Parabole *pLower = new Parabole(m, *curveVector[0], *curveVector[1], *curveVector[2]);
	curveVector.clear();
	
	
	return *new Template(*circle, *pUpper, *pLower);
}


void writeSVG(Template t, char * targetFile) {
	char meta_file[] = "metadata.svg";
	char *svg = loadSVG(meta_file);
	
	rapidxml::xml_document<> doc; 
	doc.parse<rapidxml::parse_no_data_nodes>(svg);
	
	
	XmlNode pathNode1 = doc.first_node()->first_node("g")->first_node();
	XmlNode pathNode2 = pathNode1->next_sibling();
	XmlNode pathNode3 = pathNode2->next_sibling();
	
	/*setData(pathNode1, t.c.to_string());
	 setData(pathNode2, t.pUpper.to_string());
	 setData(pathNode3, t.pLower.to_string());*/
	std::string data1 = t.c.to_string();
	std::string data2 = t.pUpper.to_string();
	std::string data3 = t.pLower.to_string();
	
	
	//const char * newValue1 = doc.allocate_string(data1.c_str(), strlen(data1.c_str()));
	rapidxml::xml_attribute<> *attr1 = pathNode1->first_attribute("d");	
	attr1->value(data1.c_str());
	
	const char * newValue2 = doc.allocate_string(data2.c_str(), strlen(data2.c_str()));
	rapidxml::xml_attribute<> *attr2 = pathNode2->first_attribute("d");	
	attr2->value(newValue2);
	
	const char * newValue3 = doc.allocate_string(data3.c_str(), strlen(data3.c_str()));
	rapidxml::xml_attribute<> *attr3 = pathNode3->first_attribute("d");		
	attr3->value(newValue3);
	
	
	std::ofstream fstr;
	fstr.open(targetFile);
	fstr << doc;
	
	fstr.close();
	
	}
};

