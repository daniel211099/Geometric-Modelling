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

Quaternion::Quaternion(const Vector axis, const double angle)
{
	Vector nAxis = getNormalizedAxis(axis);
	//Vector nAxis = axis.getNorm();
	this->Re = cos(angle / 2);
	this->Im[0] = sin(angle / 2) * nAxis[0];
	this->Im[1] = sin(angle / 2) * nAxis[1];
	this->Im[2] = sin(angle / 2) * nAxis[2];
}

double Quaternion::getAxisLength(Vector axis)
{
	double x = axis[0];
	double y = axis[1];
	double z = axis[2];
	return sqrt(x*x+y*y+z*z);
}
Vector Quaternion::getNormalizedAxis(Vector axis)
{
	double len = getAxisLength(axis);
	Vector nAxis;
	nAxis[0] = axis[0] / len;
	nAxis[1] = axis[1] / len;
	nAxis[2] = axis[2] / len;
	return nAxis;
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
Quaternion Quaternion::getComplexCon() const
{
	Quaternion q;
	q.Re = this->Re;
	q.Im = Vector(-Im[0], -Im[1], -Im[2]);
	return q;
}
Quaternion Quaternion::operator * (const Quaternion& q2)  const
{
	Quaternion q;
	q.Re	   = Re * q2.Re    - Im[0] * Im[0] - Im[1] * q2.Im[1] - Im[2] * q2.Im[2];
	double im1 = Re * q2.Im[0] + Im[0] * Re    + Im[1] * q2.Im[2] - Im[2] * q2.Im[1];
	double im2 = Re * q2.Im[1] + Im[1] * Re    + Im[2] * q2.Im[0] - Im[0] * q2.Im[2];
	double im3 = Re * q2.Im[2] + Im[2] * Re	+    Im[0] * q2.Im[1] - Im[1] * q2.Im[0];
	q.Im = Vector(im1, im2, im3);
	return q;
}




Quaternion::~Quaternion()
{
}
