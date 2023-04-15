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
	Quaternion(double re, double x, double y, double z);
	~Quaternion();

	// further necessary methods and operations...
	void setRe(double re);
	void setIm(double a, double b, double c);

	double getRe() const;
	Vector getIm() const;
	Quaternion quaternionMultiplication(Quaternion q1, Quaternion q2);
	Quaternion getComplexCon() const;

	Quaternion  operator *  (const Quaternion& q2) const;
	Quaternion  operator *  (const double& t) const;
	Quaternion  operator /  (const double& t) const;
	Quaternion  operator +  (const Quaternion& q2) const;
	Quaternion  operator ^  (const double& t)  const;

	double getAxisLength(Vector axis);
	Vector getNormalizedAxis(Vector axis);
	double getLength() const;

	void normalize();
	Quaternion inverse(const Quaternion& q);
};