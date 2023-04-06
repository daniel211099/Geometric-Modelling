////////////////////////////////////////////////////////////////////
//
//	Georg Umlauf, (c) 2012
//
////////////////////////////////////////////////////////////////////
#pragma once

#include "AffineGeometry.h"
#include "AffineMap.h"

class Quaternion 
{
private:
	double Re;	// real      part
	Vector Im;	// imaginary part

public:
	// constructors/destructors
	Quaternion();									// default constructor
	Quaternion(const Vector axis, const double angle);
	~Quaternion();

	// further necessary methods and operations...
	void setRe(double re);
	void setIm(double a, double b, double c);

	double getRe() const;
	Vector getIm() const;
	Quaternion quaternionMultiplication(Quaternion q1, Quaternion q2);
	Quaternion getComplexCon() const;

	Quaternion  operator *  (const Quaternion& q2) const;		// result  =  (*this) * v

	double getAxisLength(Vector axis);
	Vector getNormalizedAxis(Vector axis);
};