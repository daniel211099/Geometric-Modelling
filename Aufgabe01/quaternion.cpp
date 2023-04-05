////////////////////////////////////////////////////////////////////
//
//	Georg Umlauf, (c) 2012
//
////////////////////////////////////////////////////////////////////

#include "quaternion.h"
#include <iostream>

Quaternion::Quaternion()
{
	// to be done	
}

void Quaternion::setRe(double re) {
	this->Re = re;
}

void Quaternion::setIm(double a, double b, double c) {
	this->Im = Vector(a,b,c);
}

double Quaternion::getRe() const{
	return this->Re;
}

Vector Quaternion::getIm() const{
	return this->Im;
}



Quaternion::~Quaternion()
{
}
